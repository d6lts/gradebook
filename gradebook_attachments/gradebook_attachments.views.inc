<?php
// $Id$
/**
 * @file
 * Views integration with Gradebook Attachments module.
 * This is experimental and still in progress...
 */

/**
 * Database definition:
 * @code
 * $schema['gradebookapi_files'] = array(
 *    'fields' => array(
 *       'uid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *       'nid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *       'fid'         => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0),
 *       'list'        => array('type' => 'int', 'unsigned' => TRUE, 'not null' => TRUE, 'default' => 0, 'size' => 'tiny',
 *                              'description' => t('Whether the file should be visibly listed on the grade note.'), ),
 *       'description' => array('type' => 'varchar', 'length' => 255, 'not null' => TRUE),
 *     ),
 *     'indexes' => array(
 *       'nid' => array('nid'),
 *       'uid' => array('uid'),
 *       'fid' => array('fid'),
 *     ),
 *     'primary key' => array('fid', 'nid'),
 *   );
 *   return $schema;
 *  }
 * @endcode
 */

/**
 * Implement hook_views_data().
 */
function gradebook_attachments_views_data()  {
  // Basic table information.
  // ----------------------------------------------------------------
  // gradebook_attachments_assignment table
  //  New group within Views called 'Gradebook Attachments'

  $data = array();
  $data['gradebookapi_files']['table']['group']  = t('Gradebook Attachments');

  // tables + fields that can be used for SQL Joins
  $data['gradebookapi_files']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'files' => array(
      'type' => 'INNER',
      'left_field' => 'fid',
      'field' => 'fid',
    ),
    'gradebookapi_assignment' => array(
      'left_field' => 'nid',
      'field' => 'nid',
    ),
  );

  $data['gradebookapi_files']['description'] = array(
    'title' => t('File Description'),
    'help' => t('The description of the attached file.'),
    'field' => array(
      'handler' => 'views_handler_field_gradebook_attachments_description',
      'click sortable' => TRUE,
     ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
    ),
  );

  $data['gradebookapi_files']['list'] = array(
    'title' => t('Listed'),
    'help' => t('Whether or not the file is marked to be listed.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  $data['gradebookapi_files']['uid'] = array(
    'title' => t('User'),
    'help' => t('The user who attached the file.'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User'),
    ),
  );

  $data['gradebookapi_files']['nid'] = array(
    'title' => t('Assignment Node'),
    'help' => t('The assignment node associated with this file.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Assignment Node'),
    ),
  );

  return $data;
}
/**
 * Implement hook_views_data_alter().
 */
function gradebook_attachments_views_data_alter(&$data) {
  $data['node']['gradebook_attachments_fid'] = array(
    'group' => t('Gradebook Attachments'),
    'title' => t('Attached files'),
    'help' => t('All files attached to a grade assignment with gradebook_attachments.module.'),
    'real field' => 'nid',
    'field' => array(
      'handler' => 'views_handler_field_gradebook_attachments_fid',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_gradebook_attachments_fid',
      'title' => t('Has attached files'),
      'type' => 'yes-no',
      'help' => t('Only display items with attached files. This can cause duplicates if there are multiple attached files.'),
    ),
    'relationship' => array(
      'title' => t('Attached files'),
      'help' => t('Add a relationship to gain access to more file data for files uploaded by upload.module. Note that this relationship will cause duplicate nodes if there are multiple files attached to the node.'),
      'relationship table' => 'gradebookapi_files',
      'relationship field' => 'fid',
      'base' => 'files',
      'field' => 'fid',
      'handler' => 'views_handler_relationship',
      'label' => t('Files'),
    ),
  );
}

/**
 * Implement hook_views_handlers().
 */
function gradebook_attachments_views_handlers() {
  return array(
    'info' => array(
      'path' => drupal_get_path('module', 'gradebook_attachments'),
    ),
    'handlers' => array(
      'views_handler_field_gradebook_attachments_fid' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
      'views_handler_filter_gradebook_attachments_fid' => array(
        'parent' => 'views_handler_filter_boolean_operator',
      ),
      'views_handler_field_gradebook_attachments_description' => array(
        'parent' => 'views_handler_field_prerender_list',
      ),
    ),
  );
}
