<?php

/**
 * @file
 *   Provides several different handlers for exporting gradebooks.
 *   These functions were cribbed from the webform module to allow export of gradebooks
 */

/**
 * Implementation of hook_gradebook_exporters().
 *
 * Defines the exporters this module implements.
 *
 * @return
 *   An "array of arrays", keyed by content-types. The 'handler' slot
 *   should point to the PHP class implementing this flag.
 */
function gradebook_gradebook_exporters() {
  return array(
    'delimited' => array(
      'title' => t('Delimited text'),
      'description' => t('A plain text file delimited by commas, tabs, or other characters.'),
      'handler' => 'gradebook_exporter_delimited',
    ),
    'excel' => array(
      'title' => t('Microsoft Excel'),
      'description' => t('A file readable by Microsoft Excel.'),
      'handler' => 'gradebook_exporter_excel',
    ),
  );
}

/**
 * Return a list of exporters suitable for display in a select list.
 */
function gradebook_export_list() {
  $exporters = gradebook_export_fetch_definition();
  $list = array();
  foreach ($exporters as $name => $exporter) {
    $list[$name] = $exporter['title'];
  }
  return $list;
}

/**
 * Returns a Gradebook exporter definition.
 */
function gradebook_export_fetch_definition($format = NULL) {
  static $cache;
  if (!isset($cache)) {
    $cache = module_invoke_all('gradebook_exporters');
  }

  if (isset($format)) {
    if (isset($cache[$format])) {
      return $cache[$format];
    }
  }
  else {
    return $cache;
  }
}

/**
 * Instantiates a new Gradebook handler based on the format.
 */
function gradebook_export_create_handler($format, $options) {
  $definition = gradebook_export_fetch_definition($format);
  if (isset($definition) && class_exists($definition['handler'])) {
    $handler = new $definition['handler']($options);
  }
  else  {
    // TODO: Create a default broken exporter.
    $handler = new gradebook_exporter_broken($options);
  }

  return $handler;
}

class gradebook_exporter {
  function add_row(&$file_handle, $data) {
  }

  function set_headers($filename) {
    drupal_set_header("Content-Type: application/force-download");
  }

  function bof(&$file_handle) {
  }

  function eof(&$file_handle) {
  }
}

class gradebook_exporter_delimited extends gradebook_exporter {
  var $delimiter;

  function __construct($options) {
    $this->delimiter = isset($options['delimiter']) ? $options['delimiter'] : ',';
    // Convert tabs.
    if ($this->delimiter == '\t') {
      $this->delimiter = "\t";
    }
  }

  function bof(&$file_handle) {
    $output = '';

    // Include at BOM at the beginning of the file for Little Endian.
    // This makes tab-separated imports work correctly in MS Excel.
    if (function_exists('mb_convert_encoding')) {
      $output = chr(255) . chr(254);
    }
    @fwrite($file_handle, $output);
  }

  function add_row(&$file_handle, $data) {
    foreach ($data as $key => $value) {
      // Escape inner quotes and wrap all contents in new quotes.
      $data[$key] = '"'. str_replace('"', '""', $value) .'"';
    }
    $row = implode($this->delimiter, $data) ."\n";

    if (function_exists('mb_convert_encoding')) {
      $row = mb_convert_encoding($row, 'UTF-16LE', 'UTF-8');
    }

    @fwrite($file_handle, $row);
  }

  function set_headers($filename) {
    parent::set_headers($filename);

    // Convert tabs.
    if ($this->delimiter == "\t") {
      $extension = 'tsv';
      $content_type = 'text/tab-separated-values';
    }
    else {
      $extension = 'csv';
      $content_type = 'text/csv';
    }

    drupal_set_header("Content-type: $content_type");
    drupal_set_header("Content-Disposition: attachment; filename=$filename.$extension");
  }
}

/**
 * The Excel exporter currently is just a tab-delimited export.
 */
class gradebook_exporter_excel extends gradebook_exporter_delimited {
  var $delimiter;

  function gradebook_exporter_delimited($options) {
    $this->delimiter = '\t';
    parent::gradebook_exporter_delimited($options);
  }

  function set_headers($filename) {
    drupal_set_header("Content-Type: application/x-msexcel");
    drupal_set_header("Content-Disposition: attachment; filename=$filename.xls");
  }
}
