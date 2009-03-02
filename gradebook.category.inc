<?php
// $Id$
/**
 * @file
 * Gradebook callback pages associated with displaying or editing a gradebook category
 */

function gradebook_category_page($gradebook) {
  // TODO: code borrowed from forum.module
  //       needs serious cleanup

  $vid = gradebookapi_get_vid();
  $destination = drupal_get_destination();

  $header = array(t('Name'), t('Operations'));
  $vocabulary = taxonomy_vocabulary_load($vid);

  $start_from      = isset($_GET['page']) ? $_GET['page'] : 0;
  $total_entries   = 0;  // total count for pager
  $page_increment  = 25; // number of tids per page
  $displayed_count = 0;  // number of tids shown

  $tree = taxonomy_get_tree($vocabulary->vid, $gradebook->tid);
  $rows = array();
  foreach ($tree as $term) {
    $total_entries++; // we're counting all-totals, not displayed
    if (($start_from && ($start_from * $page_increment) >= $total_entries) || ($displayed_count == $page_increment)) {
      continue;
    }
    $rows[] = array(str_repeat('--', $term->depth) .' '. $term->name, l('edit', 'gradebook/'. $gradebook->tid .'/catedit/'. $term->tid, array(), $destination));
    $displayed_count++; // we're counting tids displayed
  }

  if (empty($rows)) {
    $rows[] = array(array('data' => t('No categories available.'), 'colspan' => '2'));
  }

  $GLOBALS['pager_page_array'][] = $start_from;  // FIXME
  $GLOBALS['pager_total'][] = intval($total_entries / $page_increment) + 1; // FIXME

  if ($total_entries >= $page_increment) {
    $rows[] = array(array('data' => theme('pager', NULL, $page_increment), 'colspan' => '2'));
  }

  $output = theme('table', $header, $rows, array('id' => 'taxonomy'));
  return $output;
}

function gradebook_category_add_page($gradebook) {
  return drupal_get_form('gradebook_category_form', $gradebook);
}

function gradebook_category_edit_page($gradebook, $tid=-1) {

  if ((isset($_POST['op']) && $_POST['op'] == t('Delete')) || isset($_POST['confirm'])) {
    return drupal_get_form('gradebook_confirm_category_delete', $tid, $gradebook->tid);
  }
  if ($term = taxonomy_get_term($tid)) {
    return drupal_get_form('gradebook_category_form', $gradebook, (array)$term);
  }
  return drupal_not_found();
}

/**
 *  Edit gradebook category form
 *
 *  @param $gradebook is a gradebook object
 *  @param $term is a taxonomy term object
 */

function gradebook_category_form($form_state, $gradebook, $edit = array()) {
  $form = array();
  $form['name'] = array(
    '#title' => t('Category name'),
    '#type' => 'textfield',
    '#default_value' => isset($edit['name']) ? $edit['name'] : '',
    '#maxlength' =>  64,
    '#description' => t('The category name is used to identify related assignments.'),
    '#required' => TRUE
  );

  $form['description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#default_value' => isset($edit['description']) ? $edit['description'] : '',
    '#description' => t('The category description can give users more information about the assignments it contains.')
  );

  $form['weight'] = array(
    '#type' => 'weight',
    '#title' => t('Listing Weight'),
    '#default_value' => isset($edit['weight']) ? $edit['weight'] : 0,
    '#description' => t('When listing categories, those with with light (small) weights get listed before containers with heavier (larger) weights. Categories with equal weights are sorted alphabetically.')
  );

  if (isset($edit['tid'])) {
    $parents = taxonomy_get_parents($edit['tid']);
    if (!empty($parents)) {
      $parent = array_shift($parents);
    }
    else {
      $parent = $edit['tid'];
    }
    $category_tid = $edit['tid'];
    $form['tid'] = array('#type' => 'hidden', '#value' => $category_tid);
  }
  else {
    $category_tid = NULL;
  }
  $form['parent']['#tree'] = TRUE;
  $form['parent'] = _gradebook_parent_select($gradebook->tid, $category_tid, t('Parent'));
  $form['submit' ] = array('#type' => 'submit', '#value' => t('Save'));
  if (isset($category_tid)) {
    $form['delete'] = array('#type' => 'submit', '#value' => t('Delete'));
  }
  $form['vid'] = array('#type' => 'hidden', '#value' => gradebookapi_get_vid());
  $form['parent_old'] = array('#type' => 'hidden', '#value' => isset($parent->tid) ? $parent->tid : NULL);
  $form['gid'] = array('#type' => 'hidden', '#value' => $gradebook->tid);

  return $form;
}

function gradebook_category_form_submit($form, &$form_state) {

  $status = taxonomy_save_term($form_state['values']);
  switch ($status) {
    case SAVED_NEW:
      drupal_set_message(t('Created new %type %term.', array('%term' => $form_state['values']['name'], '%type' => t('category'))));
      break;
    case SAVED_UPDATED:
      if ($form_state['values']['parent_old'] != $form_state['values']['parent']) {
        gradebookapi_calc_grades(0, $form_state['values']['parent_old'] ); // MN: $uid = 0 calc grades all users
        gradebookapi_calc_grades(0, $form_state['values']['parent']);  // MN: $uid = 0 calc grades all users
      }
      drupal_set_message(t('The %type %term has been updated.', array('%term' => $form_state['values']['name'], '%type' => t('category'))));
      break;
  }
  $form_state['redirect'] = 'gradebook/'. $form_state['values']['gid'] .'/catlist';

}

/**
 * Form builder for the category delete form.
 *
 * @ingroup forms
 */
function gradebook_confirm_category_delete(&$form_state, $tid, $gid) {
  // TODO: make sure this is a valid category term

  $term = taxonomy_get_term($tid);

  $form['tid'] = array('#type' => 'value', '#value' => $tid);
  $form['name'] = array('#type' => 'value', '#value' => $term->name);
  $form['gid'] = array('#type' => 'value', '#value' => $gid);
  return confirm_form(
    $form,
    t('Are you sure you want to delete the category %name?', array('%name' => $term->name)),
    'gradebook/'. $gid .'/catlist',
    t('Deleting a category will delete all its children if there are any. This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function gradebook_confirm_category_delete_submit($form, &$form_state) {

  taxonomy_del_term($form_state['values']['tid']);
  drupal_set_message(t('The category %term and all sub-categories and associated assignments have been deleted.', array('%term' => $form_state['values']['name'])));
  watchdog('gradebook', 'gradebook: deleted %term and all its sub-categories and associated posts.', array('%term' => $form_state['values']['name']), WATCHDOG_NOTICE);

  $form_state['redirect'] ='gradebook/'. $form_state['values']['gid'] .'/catlist';
  return;
}

