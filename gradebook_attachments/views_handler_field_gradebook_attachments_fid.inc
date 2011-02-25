<?php

/**
 * @file
 * Field handler to provide a list of file attachments.
 */
class views_handler_field_gradebook_attachments_fid extends views_handler_field_prerender_list {
  function construct() {
    parent::construct();
  }

  function allow_advanced_render() {
    return FALSE;
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_to_file'] = array('default' => FALSE);
    $options['only_listed'] = array('default' => FALSE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_to_file'] = array(
      '#title' => t('Link this field to download the file'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['link_to_file']),
    );

    $form['only_listed'] = array(
      '#title' => t('Only show "listed" file attachments'),
      '#type' => 'checkbox',
      '#default_value' => !empty($this->options['only_listed']),
    );
  }

  function pre_render($values) {
    $nids = array();
    $this->items = array();
    foreach ($values as $result) {
      $nids[] = $result->{$this->field_alias};
    }
    if ($nids) {
      // Support "only listed files" option.
      $where = '';
      if (!empty($this->options['only_listed'])) {
        $where = " AND g.list <> 0";
      }
      $result = db_query("SELECT g.nid, f.fid, f.filepath, g.description FROM {files} f INNER JOIN {gradebookapi_files} g ON f.fid = g.fid WHERE g.nid IN (" . implode(', ', $nids) . ")$where ORDER BY f.fid");
      while ($file = db_fetch_object($result)) {
        $this->items[$file->nid][$file->fid] = $this->render_link(check_plain($file->description), $file);
      }
    }
  }

  /**
   * Render whatever the data is as a link to the file.
   *
   * Data should be made XSS safe prior to calling this function.
   */
  function render_link($data, $values) {
    if ($data !== NULL && $data !== '') {
      return empty($this->options['link_to_file']) ? $data : l($data, file_create_url($values->filepath), array('html' => TRUE));
    }
    return $data;
  }
}
