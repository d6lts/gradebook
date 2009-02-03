<?php
// $Id$

/**
 * @file
 * gradebookapi.admin.inc: Administration pages for gradebookapi
 */

function gradebookapi_admin_settings() {
  $form = array();
  $form['#submit'][]= 'gradebookapi_admin_settings_submit'; // custom submit handler

  drupal_set_title(t('Gradebook API configuration'));

  $vid = gradebookapi_get_vid();
  $vocabulary = taxonomy_vocabulary_load($vid);

  $form['nodes'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Assignment Types'),
    '#multiple' => TRUE,
    '#default_value' => empty($vocabulary->nodes) ? array(): $vocabulary->nodes,
    '#options' => array_map('check_plain', node_get_types('names')),
    '#description' => t('A list of node types you want to treat as assignments.'),
  );

  $intervals = array(
    '+1 day' => t('1 day'),
    '+1 week' => t('1 week'),
    '+2 week' => t('2 weeks'),
    '+1 month' => t('1 month'),
    '+2 month' => t('2 months'),
    '+3 month' => t('3 months'),
    '+6 month' => t('6 months'),
    '+1 year' => t('1 year'),
  );

  $form['gradebookapi_assignments_interval'] = array(
    '#type' => 'radios',
    '#options' => $intervals,
    '#title' => t('Default time between creation of assignment node and due date'),
    '#default_value' => variable_get('gradebookapi_assignments_interval', '+1 month'),
    '#description' => t('This will be added to the current date to set the default due date.'),
    '#required' => FALSE,
    '#prefix' => '<div class="gradebookapi-radios">',
    '#suffix' => '</div><div class="clear-block"></div>',
  );

  $usedateapi = module_exists('date_api');
  $form['gradebookapi_use_dateapi'] = array(
    '#type' => 'checkbox',
    '#title' => t('Use Date Popup for date selection'),
    '#description' => $usedateapi ? t('Date API is enabled, check the box if you would like to use Date Popup for date selection.') : t('Date API is not available. Install and enable Date API for this feature.'),
    '#default_value' => $usedateapi ? variable_get('gradebookapi_use_dateapi', FALSE) : FALSE,
    '#disabled' => !$usedateapi,
  ); 
 
  $form['gradebookapi_assignments_date_format'] = array(
    '#type' => 'textfield',
    '#title' => t('Date format to use on gradebook page and block'),
    '#default_value' => variable_get('gradebookapi_assignments_date_format', 'm/d/Y'),
    '#description' => t('Define a php date format string like "m/d/Y". See <a href="@php-net-url">http://php.net/date</a> for more details.', array('@php-net-url' => url("http://php.net/date"))),
    '#required' => FALSE,
  );

  return system_settings_form($form);
}

function gradebookapi_admin_settings_submit($form, &$form_state) {

  $vid = gradebookapi_get_vid();
  $vocabulary = taxonomy_vocabulary_load($vid);
  $n =array_filter($form_state['values']['nodes']);
  $vocabulary->nodes = empty($n) ? array() : $n;
  $vocab_array = (array)$vocabulary;
  switch (taxonomy_save_vocabulary($vocab_array) ) {  // must pass array in! not vocab object!
    case SAVED_NEW:
      drupal_set_message(t('Created new vocabulary %name.', array('%name' => $vocabulary->name)));
      watchdog('gradebook', 'Created new vocabulary %name.', array('%name' => $vocabulary->name), WATCHDOG_NOTICE, l(t('edit'), 'admin/content/taxonomy/edit/vocabulary/'. $vid));
      break;
    case SAVED_UPDATED:
      drupal_set_message(t('Updated vocabulary %name.', array('%name' => $vocabulary->name)));
      watchdog('gradebook', 'Updated vocabulary %name.', array('%name' => $vocabulary->name), WATCHDOG_NOTICE, l(t('edit'), 'admin/content/taxonomy/edit/vocabulary/'. $vid));
      break;
  }
  $form_state['redirect'] = 'admin/gradebook/gradebookapi';

}
