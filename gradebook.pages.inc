<?php
// $Id$

/**
 * @file
 * Gradebook callback pages associated with displaying or editing a grade in the gradebook
 */

/**
 *  Provide either an assignment summary (if $user is a student)
 *  or an edit form (if $user is a teacher)
 *
 *  @param $gradebook
 *    the $gradebook object
 *  @param $uid
 *    path argument for student user id whose grade we want to edit/view
 *  @param $nid
 *    parth argument for the node id for the assignment we want to edit/view
 *
 */

function gradebook_grade_page($gradebook, $uid, $nid) {  //all users with 'access gradebook' permissions can view this page
  $account = user_load(array('uid' => $uid));

  if ($account !== FALSE && gradebookapi_is_student($gradebook, $account)) {
    $types = gradebookapi_get_assignment_types();
    $node = node_load($nid);
    if (in_array($node->type, $types)) {
      $grade = gradebookapi_get_grade($uid, $nid);
      $grades[$uid] = (array)$grade;
      return drupal_get_form('gradebook_grade_form', $gradebook, $grades);
    }
  }
  drupal_not_found();
}

/**
 *  Enter grades for all students by assignment
 *    This form is only accessible to teachers
 *  @param $gradebook
 *    the $gradebook object
 *  @param $nid
 *    path argument for the node id for the assignment we want to edit/view
 *
 */

function gradebook_grade_assignment_page($gradebook, $nid) {  // only accessed by teacher
  $types = gradebookapi_get_assignment_types();
  $node = node_load($nid);
  if (in_array($node->type, $types)) {
    $grades = array();
    $students = array();
    $students = gradebookapi_get_students($gradebook);
    foreach ($students['uid'] as $uid) {
      $grade = gradebookapi_get_grade($uid, $nid);  //now its safe to get $grade
      $grades[$uid] = (array)$grade;
    }
    return drupal_get_form('gradebook_grade_form', $gradebook, $grades);
  }
  drupal_not_found();
}

function gradebook_grade_student_page($gradebook, $uid) {  // access is limited to teacher through menu system
  // get sort order for query
  $sort = (isset($_GET['sort']) && ($_GET['sort'] == 'asc')) ? 'desc' : 'asc';

  $account = user_load(array('uid' => $uid));
  if (gradebookapi_is_student($gradebook, $account)) {
    // get assignments
    $tids = array();
    $assignments = gradebook_get_sorted_assignments($gradebook, $tids, $order, TRUE, $sort);

    // get grades for assignments
    $grades = array();
    foreach ($assignments as $assignment) {
      $grades[$assignment->nid] = (array)gradebookapi_get_grade($account->uid, $assignment->nid);
    }
    return drupal_get_form('gradebook_grade_form', $gradebook, $grades);
  }
  drupal_not_found();
}

function gradebook_grade_form($form_state, $gradebook, $grades) {
  global $user;
  
  $teacher = gradebookapi_is_teacher($gradebook);
  $students = gradebookapi_get_students($gradebook);
  $suid = array_flip($students['uid']); // this is needed because keys of $students is lost in module_invoke_all call

  $form = array('#theme' => 'gradebook_grade_form');
  $form['grade'] = array('#tree' => TRUE);
  $form['#grades'] = $grades;  // make available for hook_form_alter
  foreach ($grades as $id => $grade) {
//    $account = user_load(array('uid' => $grade['uid']));
    $uid = $grade['uid'];
    $display_name = $students['name'][$suid[$uid]];  // this is needed because module_invoke_all looses the keys

    $node = node_load($grade['nid']);
    $form['grade'][$id]['#node'] = $node;  // make available for hook_form_alter
    $form['grade'][$id]['uid'] = array('#type' => 'value', '#value' => $uid);  // must be '#type' => value
    $form['grade'][$id]['nid'] = array('#type' => 'value', '#value' => $node->nid);     // must be '#type' => value
    $form['grade'][$id]['user'] = array('#value' => $display_name);
    $form['grade'][$id]['node'] = array('#value' => $node->title);
    $form['grade'][$id]['#possible'] = $node->possible;
    $percentage = ($grade['earned'] == '') ? NULL : ($node->possible != 0) ? 100.0*$grade['earned']/$node->possible : 0;
    $form['grade'][$id]['#oldpercentage'] = $percentage;
    $form['grade'][$id]['#oldearned'] = $grade['earned'];
    $form['grade'][$id]['timestamp'] = array('#type' => 'value', '#value' => $grade['timestamp']);

    if ($teacher) {  // display edit form
      $form['grade'][$id]['earned'] = array('#type' => 'textfield', '#size' => 5, '#default_value' => $grade['earned'], '#field_suffix' => '/'. $node->possible);
      $form['grade'][$id]['percentage'] = array('#type' => 'textfield', '#size' => 5, '#default_value' => $percentage, '#field_suffix' => '%');
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#default_value' => $grade['exempt']);
      $form['grade'][$id]['note'] = array('#type' => 'textarea', '#cols' => 40, '#rows' => 6, '#default_value' => $grade['note']);
    }
    else { // display grades without option for editing
      if ($grade['earned'] != '') {
        $grade_mark = gradebookapi_load_marks($gradebook->tid, $node->nid, $percentage, FALSE);
        if (!$grade_mark) {
          $grade_mark = $grade['earned'];
          $suffix =  '/'. $node->possible;
        }
      $form['grade'][$id]['earned'] = array('#value' => $grade_mark, '#suffix' => $suffix);
      $form['grade'][$id]['percentage'] = array('#value' => $percentage, '#suffix' => '%');
      }
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#value' => $grade['exempt'], '#disabled' => TRUE);
      $form['grade'][$id]['note']   = array('#value' => $grade['note']);
    }
  }
  $modules = module_implements('gradebook_form_add_elements');
  foreach ($modules as $module) {
    $func = $module .'_gradebook_form_add_elements';
    $func($form, $form_state, $grades, $gradebook);
  }
  $form['return_url'] = array('#type' => 'value', '#value' => 'gradebook/'. $gradebook->tid);
  $form['teacher'] = array('#type' => 'value', '#value' => $teacher);
  if ($teacher) {
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save All'));
  }
  $form['cancel'] = array('#type' => 'submit', '#value' =>  t('Return to Gradebook'), '#submit' => array('gradebook_grade_form_cancel'));

  return $form;
}

/*
 * No validation is needed for floating point grades, but saving this function for future development.
 */
function gradebook_grade_form_validate($form, &$form_state) {
// do nothing
}

function gradebook_grade_form_cancel($form, &$form_state) {
    unset($form_state['storage']);  // clear the storage bins now that files have been saved - needed for redirect
    $form_state['redirect'] = $form_state['values']['return_url'];
    unset($form_state['values']['return_url']);
}

function gradebook_grade_form_submit($form, &$form_state) {
    $keys = array_keys($form_state['values']['grade']);
    $count = 0;
    foreach ($keys as $key) {
      $grade = (object)$form_state['values']['grade'][$key];
      if ($form_state['values']['teacher']) {
        $node = node_load($form_state['values']['grade'][$key]['nid']);
        $grade->due_date = $node->due_date;
        $grade->publish_date = $node->publish_date;
        $percentage = $form_state['values']['grade'][$key]['percentage'];
        $earned = $form_state['values']['grade'][$key]['earned'];
        if ($earned != $form['grade'][$key]['#oldearned']) {// use which one changed... if both changed, use points...
          $grade->earned = $earned;
        }
        elseif ($percentage != $form['grade'][$key]['#oldpercentage']) {
          $grade->earned = $percentage*$form['grade'][$key]['#possible']/100.0;
        }
        gradebookapi_set_grade($grade);
      }
      else {
        if ($grade->timestamp == 0) {  // grade doesn't exist in gradebookapi_grade, save a null grade
          gradebookapi_set_grade($grade);
        }
        else { // grade already exists, but give other modules a chance to do something here.
          module_invoke_all('gradebookapi_grade', 'set', $grade);
        }
      }
      ++$count;
    }
    unset($form_state['storage']);
    if ($count >0) {
      drupal_set_message(t('Saved %num %type.', array('%num' => $count, '%type' => t('grades'))));
    }
    else {
      drupal_set_message(t('No changes were made to grades'));
    }
    $form_state['redirect'] = $form_state['values']['return_url'];
    unset($form_state['values']['return_url']);
}

function theme_gradebook_grade_form(&$form) {
  global $user;
  $ids = element_children($form['grade']);
  $selectid = count($ids);
  $header = array();
  if ($selectid > 1) {
    $header = array('select' => t('Select'));  // don't show column heading unless there is more than one choice
  }
  $header += array('name' => t('Name'), 'title' => t('Assignment Title'), 'grade' => t('Grade'), 'exempt' => t('Exempt'), 'note' => t('Note'));
  foreach ($ids as $id) {
    $row = NULL;
    if  ($selectid > 1) {  // don't show radio button for a single entry
      $row[] = array('data' => drupal_render($form['gradeid'][$id]), 'class' => 'select');
    }
    $row[] = array('data' => drupal_render($form['grade'][$id]['user']), 'class' => 'name');
    // allow modules to add information to the title block
    $module_output = _gradebook_module_additions('title', $form, $id, $form['teacher']['#value'], '<br />');
    $row[] = array('data' => drupal_render($form['grade'][$id]['node']) .'<br />'. $module_output, 'class' => 'title');
    if (isset($form['grade'][$id]['earned'])) {
      $row[] = array('data' => drupal_render($form['grade'][$id]['earned']) .'<br />'. drupal_render($form['grade'][$id]['percentage']), 'class' => 'earned-possible');
    }
    else {
      $row[] = array();
    }
    $row[] = array('data' => drupal_render($form['grade'][$id]['exempt']), 'class' => 'exempt-box');
    $output = '<div class="note">'. drupal_render($form['grade'][$id]['note']) .'</div>';
    // give modules a chance to add output below the note block
    // can't use module_invoke, module_invoke_all, or call_user_func, because these do not pass arguemtns by reference!

    $output .= _gradebook_module_additions('note', $form, $id, $form['teacher']['#value']);
    $row[] = array('data' => $output);
    $rows[] = array('data' => $row);
  }
  $output = theme('table', $header, $rows, array('class' => 'gradebook'));
  $output .= theme('pager', NULL, variable_get('gradebook_grades_per_page', 10), 0);  // configure at admin screen!
  $output .= '<div>'. _gradebook_module_additions('bottom', $form, -1, $form['teacher']['#value']) .'</div>';
  $output .= '<div>'. drupal_render($form['submit']) . drupal_render($form['cancel']) .'</div>';
  $output .= drupal_render($form);
  return $output;
}

function _gradebook_module_additions($position, &$form, $id, $teacher, $separator = '') {
  $output = '';
  $modules = module_implements('gradebook_theme_add_content');
  foreach ($modules as $module) {
    $func = $module .'_gradebook_theme_add_content';
    $output .= $func($position, $form, $id, $teacher);  // need to pass by reference!
    $output .= $separator;
  }
  return $output;
}

function gradebook_rebuild_page($gradebook) {
  gradebookapi_rebuild_grades($gradebook->tid);
  drupal_goto('gradebook/'. $gradebook->tid);
}
