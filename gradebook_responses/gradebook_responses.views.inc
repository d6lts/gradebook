<?php
// $Id$
/**
 * @file
 * Views integration with gradebook_responses module.
 * This is experimental and still in progress...
 */

/*
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
    'user' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
    'aid' => array(
      'left_field' => 'aid',
      'field' => 'aid',
    ),
  );

  $data['gradebook_responses']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node associated with the response.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
    ),
  );

  $data['gradebook_responses']['uid'] = array(
    'title' => t('User'),
    'help' => t('The user who created the response.'),
    'relationship' => array(
      'base' => 'users',
      'field' => 'uid',
      'handler' => 'views_handler_relationship',
      'label' => t('User'),
    ),
  );

  $data['gradebook_responses']['aid'] = array(
    'title' => t('Response Node'),
    'help' => t('The node associated with the response.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'aid',
      'handler' => 'views_handler_relationship',
      'label' => t('Response Node'),
    ),
  );

  return $data;
}
