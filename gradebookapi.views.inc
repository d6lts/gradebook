<?php
/**
 * @file
 * Views integration with Gradebook module.
 * This is experimental and still in progress...
 */

/**
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
 *
 *  $schema['gradebookapi_grade'] = array(
 *    'fields' => array(
 *      'uid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'nid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'earned'      => array('type' => 'float', 'unsigned' => FALSE, 'not null' => FALSE),
 *      'exempt'      => array('type' => 'int', 'unsigned' => FALSE, 'not null' => TRUE, 'size' => 'tiny', 'default' => 0),
 *      'timestamp'   => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'note'        => array('type' => 'text', 'unsigned' => FALSE, 'not null' => FALSE, 'size' => 'big'),
 *     ),
 *    'primary key' => array('nid', 'uid'),
 *  );
 *
 * @endcode
 */

/**
 * Implement hook_views_data().
 */
function gradebookapi_views_data()  {
  // Basic table information.
  // ----------------------------------------------------------------
  // gradebookapi_assignment table
  //  New group within Views called 'Gradebook Assignment'

  $data = array();
  $data['gradebookapi_assignment']['table']['group']  = t('Gradebook Assignment');

  $data['gradebookapi_assignment']['table']['base'] = array(
    'field' => 'nid',
    'title' => t('Gradebook Assignment'),
    'help' => t("Information about an assignment."),
    'weight' => -10,
  );
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
      'handler' => 'gradebookapi_views_handler_field_numeric',
      'click sortable' => TRUE,
      'float' => TRUE,
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
    'user' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'gradebookapi_assignment' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['gradebookapi_grade']['earned'] = array(
    'title' => t('Earned'),
    'help' => t('Points earned for assignment.'),
    'field' => array(
      'handler' => 'gradebookapi_views_handler_field_numeric',
      'click sortable' => TRUE,
      'float' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['gradebookapi_grade']['exempt'] = array(
    'title' => t('Exempt'),
    'help' => t('Student is exempted from this assignment.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['gradebookapi_grade']['note'] = array(
    'title' => t('Note'),
    'help' => t('Text explaining grade to the student.'),
    'field' => array(
      'handler' => 'views_handler_field_markup',
      'format' => FILTER_FORMAT_DEFAULT,

    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );

  // Graded date.
  $data['gradebookapi_grade']['timestamp'] = array(
    'title' => t('Date graded'),
    'help' => t('Date on which the grade was last updated.'),
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

  $data['gradebookapi_grade']['uid'] = array(
    'title' => t('Student'),
    'help' => t('The student associated with the grade.'),
    'field' => array(
      'handler' => 'views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_user_uid',
      'name field' => 'name', // display this field in the summary
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Student'),
    ),
   'filter' => array(
      'title' => t('Student Name'),
      'handler' => 'gradebookapi_views_handler_filter_user_grade',
    ),
  );

  $data['gradebookapi_grade']['nid'] = array(
    'title' => t('Assignment node'),
    'help' => t('The node associated with the grade.'),
    'relationship' => array(
      'base' => 'node',
      'type' => 'INNER',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Assignment node'),
    ),
  );

  return $data;
}

/**
 * Implement hook_views_handlers() 
 */
function gradebookapi_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gradebook'),
    ),
    'handlers' => array(
      'gradebookapi_views_handler_filter_user_grade' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'gradebookapi_views_handler_field_numeric' => array(
        'parent' => 'views_handler_field_numeric',
      ),
    ),
  );
}

/**
 * Implement hook_date_api_fields().
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
  if ($field == 'gradebookapi_grade.timestamp') {
    return array(
      'sql_type' => DATE_UNIX, // DATE_ISO or DATE_UNIX or DATE_DATETIME
      'tz_handling' => 'site', // 'site' or 'none' or 'date' or 'utc'
    );
  }
}
