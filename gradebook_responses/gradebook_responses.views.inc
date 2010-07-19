<?php
// $Id$
/**
 * @file
 * Views integration with the gradebook_responses module.
 * This is experimental and still in progress...
 */

/**
 * Implement hook__views_data().
 *
 * Database definition:
 * @code
 *  $schema['gradebook_responses'] = array(
 *    'fields' => array(
 *      'nid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'aid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'uid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *      'rstatus'     => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0, 'size' => 'tiny'),
 *    ),
 *    'indexes' => array(
 *      'aid' => array('aid'),
 *      'uid' => array('uid'),
 *    ),
 *    'primary key' => array('nid'),
 *  );
 * @endcode
 */

function gradebook_responses_views_data()  {
  // Basic table information.
  // ----------------------------------------------------------------
  // gradebook_responses table
  //  New group within Views called 'Gradebook Responses'

  $data = array();
  $data['gradebook_responses']['table']['group']  = t('Gradebook Responses');

  // tables + fields that can be used for SQL Joins
  $data['gradebook_responses']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'gradebookapi_assignment' => array(
      'left_field' => 'nid',
      'field' => 'aid',
    ),
  );

 // Response Status.
  $data['gradebook_responses']['rstatus'] = array(
    'title' => t('Response Status'),
    'help' => t('Shows progress toward completing the response.'),
    'field' => array(
      'handler' => 'views_handler_field_response_status',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_response_status',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );
  $data['gradebook_responses']['nid'] = array(
    'title' => t('Response Node'),
    'help' => t('The node responding to the assignment.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Response Node'),
    ),
  );
  $data['gradebook_responses']['aid'] = array(
    'title' => t('Assignment Node'),
    'help' => t('The assignment node associated with the response.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Assignment Node'),
    ),
  );
  $data['gradebook_responses']['uid'] = array(
    'title' => t('Responding Student'),
    'help' => t('The student who authored the response.'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('Responding Student'),
    ),
  );

  return $data;
}

/**
 * Implement hook_views_handlers() 
 */
function gradebook_responses_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gradebook_responses'),
    ),
    'handlers' => array(
      'views_handler_field_response_status' => array(
        'parent' => 'views_handler_field',
      ),
      'views_handler_filter_response_status' => array(
        'parent' => 'views_handler_filter_in_operator',
      ),
    ),
  );
}
