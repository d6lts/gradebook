<?php
// $Id$
/**
 * This file includes helper functions for creating reports for gradebook.module
 * This is code originally from webform - modified to suit gradebook needs
 */

/**
 * Form to configure the download of CSV files.
 */
function gradebook_download_form(&$form_state, $gradebook) {
  require_once('gradebook_export.inc');

  $form = array();

  $form['#gradebook'] = $gradebook;
  $options = gradebook_export_pages();
  $default = array_shift(array_keys($options));
    $form['func'] = array(
    '#type' => 'radios',
    '#title' => t('Page to Export'),
    '#options' => $options,
    '#default_value' => $default,
    '#required' => TRUE,
  );

  $form['format'] = array(
    '#type' => 'radios',
    '#title' => t('Export format'),
    '#options' => gradebook_export_list(),
    '#default_value' => variable_get('gradebook_export_format', 'delimited'),
  );

  $form['delimiter'] = array(
    '#type' => 'select',
    '#title' => t('Delimited text format'),
    '#description' => t('This is the delimiter used in the CSV/TSV file when downloading a gradebook. Using tabs in the export is the most reliable method for preserving non-latin characters. You may want to change this to another character depending on the program with which you anticipate importing results.'),
    '#default_value' => variable_get('gradebook_csv_delimiter', '\t'),
    '#options' => array(
      ','  => t('Comma (,)'),
      '\t' => t('Tab (\t)'),
      ';'  => t('Semicolon (;)'),
      ':'  => t('Colon (:)'),
      '|'  => t('Pipe (|)'),
      '.'  => t('Period (.)'),
      ' '  => t('Space ( )'),
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Download'),
  );

  return $form;
}

function gradebook_download_form_submit(&$form, &$form_state) {
  gradebook_download($form['#gradebook'], $form_state['values']['func'], $form_state['values']['format'], $form_state['values']);
}

/**
 * Return a list of possible pages to export for display in a select list.
 */
function gradebook_export_pages() {
  $pages = module_invoke_all('gradebook_export');
  
  foreach ($pages as $func => $title) {
    $list[$func] = $title;
  }
  return $list;
}


/**
 * Generate a Excel-readable CSV file containing all submissions for a Gradebook.
 *
 * The CSV requires that the data be presented in a flat file.
 * A couple of header rows are added to the normal gradebook_gradebook_page table
 * to help identify the table.
 */
function gradebook_download($gradebook, $gradebook_page, $format = 'delimiter', $options = array()) {
  require_once('gradebook_export.inc');

  if (empty($options)) {
    $options = array(
      'delimiter' => variable_get('gradebook_csv_delimiter', '\t'),
      'format' => variable_get('gradebook_export_format', 'delimited'),
    );
  }

  // Open a new Gradebook exporter object.
  $exporter = gradebook_export_create_handler($format, $options);
  $temp = file_directory_temp();
  $file_name = tempnam($temp, 'gradebook');
  $handle = @fopen($file_name, 'w'); // The @ suppresses errors.
  $exporter->bof($handle);
  $header = array();
  $header[] = array(check_plain($gradebook->name));
  $header[] = array(t('Downloaded on:'), format_date(time(), 'custom', "F j, Y, g:i a"));

  $gradebook_table = $gradebook_page($gradebook, $uid = NULL, $export = TRUE);
  $gradebook_table = array_merge($header, $gradebook_table);

  // Add rows to the file.
  foreach ($gradebook_table as $row) {
    $exporter->add_row($handle, $row);
  }
  // Add the closing bytes.
  $exporter->eof($handle);

  // Close the file.
  @fclose($handle);

  $export_name = check_plain($gradebook->name);
  $exporter->set_headers($export_name);
  @readfile($file_name);  // The @ makes it silent.
  @unlink($file_name);  // Clean up, the @ makes it silent.
  exit();
}
