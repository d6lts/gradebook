<?php
// $Id$

/**
 * Filter handler to select user grade using a node's uid.
 */
class gradebookapi_views_handler_filter_user_grade extends views_handler_filter_boolean_operator {
  function construct() {
    parent::construct();
    $this->value_value = t('Is the node creator');
  }

  function query() {
    $table = $this->ensure_my_table();
    $this->query->add_where($this->options['group'], "$this->table_alias.$this->real_field " . (empty($this->value) ? '!=' : '=') . " " . $this->query->base_table . ".$this->real_field");
  }
}
