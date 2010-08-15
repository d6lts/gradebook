<?php
// $Id$

/**
 * @file
 * Filter handler to select user grade using a node's uid.
 */

class gradebookapi_views_handler_filter_user_grade extends views_handler_filter_boolean_operator {
  function construct() {
    parent::construct();
    $this->value_value = t('Is the node creator');
  }

function option_definition() {
    $options = parent::option_definition();

    $options['filternull'] = array('default' => TRUE);

    return $options;
  }

 function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['filternull'] = array(
      '#type' => 'checkbox',
      '#title' => t('Remove results where the student does not have an assigned grade.'),
      '#default_value' => $this->options['filternull'],
    );
  }

  function query() {
    $table = $this->ensure_my_table();
    $where = "$this->table_alias.$this->real_field " . (empty($this->value) ? '!=' : '=');
    $where .= " " . $this->query->base_table . ".$this->real_field";
    if (!$this->options['filternull']) {
      $where = "($this->table_alias.$this->real_field IS NOT NULL AND " . $where;
      $where .= ") OR $this->table_alias.$this->real_field IS NULL";
    }
    $this->query->add_where($this->options['group'], $where);
  }
}
