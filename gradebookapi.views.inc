<?php
// $Id$
/**
 * @file
 * Views integration with Gradebook module.
 * This is experimental and still in progress...
 */

/*
 *
 * Database definition:
 * @code
 *  $schema['gradebookapi_assignment'] = array(
 *    'fields' => array(
 *      'nid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'possible'    => array('type' => 'float', 'unsigned' => FALSE, 'not null' => TRUE, 'default' => 0),
 *      'publish_date' => array( 'description' => 'Date assignment becomes effective.', 'type' => 'int',
 *        'unsigned' => FALSE, 'not null' => TRUE, 'default' => 0, 'disp-width' => '10',),
 *      'due_date' => array('description' => 'Date assignment is due.', 'type' => 'int', 'unsigned' => FALSE,
 *        'not null' => TRUE, 'default' => 0, 'disp-width' => '10', ),
 *      'sid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => FALSE),
 *    ),
 *    'primary key' => array('nid'),
 *  );
 * @endcode
 */

/**
 * Implementation of hook_views_data().
 */
function gradebookapi_views_data()  {
  // Basic table information.
  // ----------------------------------------------------------------
  // gradebookapi_assignment table
  //  New group within Views called 'Gradebook Assignment'

  $data = array();
  $data['gradebookapi_assignment']['table']['group']  = t('Gradebook Assignment');

  // tables + fields that can be used for SQL Joins
  $data['gradebookapi_assignment']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  // Due_date.
  $data['gradebookapi_assignment']['due_date'] = array(
    'title' => t('Due Date'),
    'help' => t('Date on which the Gradebook Assignment is due.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Publish_date.
  $data['gradebookapi_assignment']['publish_date'] = array(
    'title' => t('Publish Date'),
    'help' => t('Date on which the Gradebook Assignment was published.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Possible.
  $data['gradebookapi_assignment']['possible'] = array(
    'title' => t('Possible'),
    'help' => t('Possible points for assignment.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['gradebookapi_grade']['table']['group']  = t('Gradebook Grade');

  // Tables + fields that can be used for SQL joins.
  $data['gradebookapi_grade']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'gradebookapi_assignment' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),

  );
  $data['gradebookapi_grade']['uid'] = array(
    'title' => t('User'),
    'help' => t('The user associated with the grade.'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User'),
    ),
  );

  $data['gradebookapi_grade']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node associated with the grade.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
    ),
  );
  return $data;
}

/**
 * Implementation of hook_date_api_fields().
 */
function gradebookapi_date_api_fields($field) {
  if ($field == 'gradebookapi_assignment.publish_date') {
    return array(
      'sql_type' => DATE_UNIX, // DATE_ISO or DATE_UNIX or DATE_DATETIME
      'tz_handling' => 'site', // 'site' or 'none' or 'date' or 'utc'
    );
  }
  if ($field == 'gradebookapi_assignment.due_date') {
    return array(
      'sql_type' => DATE_UNIX, // DATE_ISO or DATE_UNIX or DATE_DATETIME
      'tz_handling' => 'site', // 'site' or 'none' or 'date' or 'utc'
    );
  }
}
