<?php
// $Id$

/**
 * @file
 * Gradebook callback pages associated with displaying or editing a grade in the gradebook
 */

/**
 * Menu callback provides either an assignment summary (if user is a student)
 * or an edit form (if user is a teacher).
 *
 * All users with 'access gradebook' permissions can view this page.
 *
 * @param $gradebook
 *   The $gradebook object.
 * @param $uid
 *   Path argument for student user id whose grade we want to edit/view.
 * @param $nid
 *   Path argument for the node id for the assignment we want to edit/view.
 *
 * @return
 *   The grade edit or summary form.
 */
function gradebook_grade_page($gradebook, $uid, $nid) {
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
 * Menu callback to enter grades for all students by assignment.
 *
 *   This form is only accessible to teachers.
 *
 * @param $gradebook
 *   The $gradebook object.
 * @param $nid
 *   Path argument for the node id for the assignment we want to edit/view.
 *
 * @return
 *   The grade edit or summary form.
 */
function gradebook_grade_assignment_page($gradebook, $nid) {
  $types = gradebookapi_get_assignment_types();
  $node = node_load($nid);
  if (in_array($node->type, $types)) {
    $grades = array();
    $students = array();
    $students = gradebookapi_get_students($gradebook);
    foreach ($students['uid'] as $uid) {
      $grade = gradebookapi_get_grade($uid, $nid);
      $grades[$uid] = (array)$grade;
    }
    return drupal_get_form('gradebook_grade_form', $gradebook, $grades);
  }
  drupal_not_found();
}

/**
 * Menu callback to edit or provide a summary of student grades.
 * 
 * Access is limited to teacher through the menu system.
 *
 * @param $gradebook
 *   The gradebook object.
 * @param $uid
 *   The user id.
 *
 * @return
 *   The grade edit or summary form.
 */
function gradebook_grade_student_page($gradebook, $uid) {  // access is limited to teacher through menu system
  // get sort order for query
  $default_sort = variable_get('gradebook_page_sort-'. $gradebook->tid, 'asc');
  $sort = (isset($_GET['sort']) && ($_GET['sort'] == 'asc')) ? 'asc' : 'desc';
  $default_order = variable_get('gradebook_page_order-'. $gradebook->tid, 'category');
  $order = isset($_GET['order']) ? $_GET['order'] : $default_order;

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

/**
 * Form builder to edit a student grade.
 *
 * @param $gradebook
 *   The gradebook object.
 * @param $grades
 *   An array of grade objects.
 *
 * @see gradebook_grade_form_validate()
 * @see gradebook_grade_form_cancel()
 * @see gradebook_grade_form_submit()
 * @ingroup forms
 */
function gradebook_grade_form($form_state, $gradebook, $grades) {
  global $user;
  
  $teacher = gradebookapi_is_teacher($gradebook);
  $students = gradebookapi_get_students($gradebook);
  $suid = array_flip($students['uid']); // This is needed because keys of $students is lost in module_invoke_all call.

  $form = array('#theme' => 'gradebook_grade_form');
  $form['grade'] = array('#tree' => TRUE);
  $form['#grades'] = $grades;  // Make grades available for hook_form_alter
  $form['#gradebook'] = $gradebook; // Make gradebook available for hook_form_alter
  foreach ($grades as $id => $grade) {
  //    $account = user_load(array('uid' => $grade['uid']));
    $uid = $grade['uid'];
    $display_name = $students['name'][$suid[$uid]];  // This is needed because module_invoke_all loses the keys.
    $oldearned = isset($grade['earned']) ? $grade['earned'] : NULL;
    $oldexempt = isset($grade['exempt']) ? $grade['exempt'] : 0;
    $oldnote = isset($grade['note']) ? $grade['note'] : '';
    // Make the node object available for hook_form_alter.
    $node = node_load($grade['nid']);
    $form['grade'][$id]['#node'] = $node;
    $form['grade'][$id]['uid'] = array('#type' => 'value', '#value' => $uid);  // must be '#type' => 'value'
    $form['grade'][$id]['nid'] = array('#type' => 'value', '#value' => $node->nid); // must be '#type' => 'value'
    $form['grade'][$id]['user'] = array('#value' => $display_name);
    $form['grade'][$id]['node'] = array('#value' => $node->title);
    $form['grade'][$id]['#possible'] = $node->possible;
    $percentage = (!isset($grade['earned'])) ? NULL : (isset($node->possible) && $node->possible != 0) ? 100.0*$grade['earned']/$node->possible : 0;
    $form['grade'][$id]['#oldpercentage'] = $percentage;
    $form['grade'][$id]['#oldearned'] = $oldearned;
    $form['grade'][$id]['timestamp'] = array('#type' => 'value', '#value' => $grade['timestamp']);
    if ($teacher) {
      // Displays an edit form.
      $form['grade'][$id]['earned'] = array('#type' => 'textfield', '#size' => 4, '#default_value' => $oldearned, '#field_suffix' => '/' . $node->possible);
      $form['grade'][$id]['percentage'] = array('#type' => 'textfield', '#size' => 4, '#default_value' => $percentage, '#field_suffix' => '%');
      
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#default_value' => $oldexempt);
      
      $form['grade'][$id]['note'] = array('#type' => 'textarea', '#cols' => 40, '#rows' => 6, '#default_value' => $oldnote);
    }
    else {
      // Displays grades without an option for editing.
      if ($oldearned != '') {
        $grade_mark = gradebookapi_load_marks($gradebook->tid, $node->nid, $percentage, FALSE);
        if (!$grade_mark) {
          $grade_mark = $oldearned;
          $suffix =  '/' . $node->possible;
        }
      $form['grade'][$id]['earned'] = array('#value' => $grade_mark, '#suffix' => $suffix);
      $form['grade'][$id]['percentage'] = array('#value' => $percentage, '#suffix' => '%');
      }
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#value' => $oldexempt, '#disabled' => TRUE);
      $form['grade'][$id]['note'] = array('#value' => $oldnote);
    }
  }
  $modules = module_implements('gradebook_form_add_elements');
  foreach ($modules as $module) {
    $func = $module . '_gradebook_form_add_elements';
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

function gradebook_grade_form_validate($form, &$form_state) {
  // No validation is needed for floating point grades, but saving this function for future development.
}

function gradebook_grade_form_cancel($form, &$form_state) {
  // Clear the storage bins now that files have been saved - needed for redirect.
  unset($form_state['storage']);
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
      if ($form['grade'][$key]['percentage']['#disabled'] || $form['grade'][$key]['earned']['#disabled']) {
        $grade->earned = $form['grade'][$key]['#oldearned']; 
        $grade->percentage = $form['grade'][$key]['#oldpercentage'];
      }
      else {
        $percentage = $form_state['values']['grade'][$key]['percentage'];
        $earned = $form_state['values']['grade'][$key]['earned'];
        // Use whichever changed... if both changed, use points.
        if ($earned != $form['grade'][$key]['#oldearned']) {
          $grade->earned = $earned;
        }
        elseif ($percentage != $form['grade'][$key]['#oldpercentage']) {
          $grade->earned = $percentage*$form['grade'][$key]['#possible']/100.0;
        }
      }
      gradebookapi_set_grade($grade);
    }
    else {
      if ($grade->timestamp == 0) {
        // Grade doesn't exist in gradebookapi_grade, save a null grade.
        gradebookapi_set_grade($grade);
      }
      else { // Grade already exists, but give other modules a chance to do something here.
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

/**
 * Returns HTML to theme the grade form.
 *
 * @return
 *   The HTML string ready to display.
 *
 * @ingroup themeable
 */
function theme_gradebook_grade_form($form) {
  global $user;
  $ids = element_children($form['grade']);
  $selectid = count($ids);
  $header = array();
  // Don't show column heading unless there is more than one choice.
  if ($selectid > 1) {
    $header = array('select' => t('Select'));
  }
  $header += array('name' => t('Name'), 'title' => t('Assignment Title'), 'grade' => t('Grade'), 'exempt' => t('Exempt'), 'note' => t('Note'));
  foreach ($ids as $id) {
    $row = NULL;
    if  ($selectid > 1) {  // Don't show radio button for a single entry
      $row[] = array('data' => drupal_render($form['gradeid'][$id]), 'class' => 'select');
    }
    $row[] = array('data' => drupal_render($form['grade'][$id]['user']), 'class' => 'name');
    // allow modules to add information to the title block
    $data = drupal_render($form['grade'][$id]['node']);
    $module_output = _gradebook_module_additions('title', $form, $id, $form['teacher']['#value'], '<br />');
    if ($module_output != '') {
      $data .= '<br />' . $module_output;
    }
    $row[] = array('data' => $data , 'class' => 'title');
    if (isset($form['grade'][$id]['earned'])) {
      $row[] = array('data' => drupal_render($form['grade'][$id]['earned']) . '<br />' . drupal_render($form['grade'][$id]['percentage']), 'class' => 'earned-possible');
    }
    else {
      $row[] = array('data' => '&nbsp', 'class' => 'earned-possible');
    }
    $row[] = array('data' => drupal_render($form['grade'][$id]['exempt']), 'class' => 'exempt-box');
    $output = '<div class = "note-region">' . drupal_render($form['grade'][$id]['note']) . '</div>';
    // Give modules a chance to add output below the note block.
    // Can't use module_invoke, module_invoke_all, or call_user_func, because these do not pass arguments by reference!
    $output .= _gradebook_module_additions('note', $form, $id, $form['teacher']['#value']);
    $row[] = array('data' => $output, 'class' => 'note');
    $rows[$id] = array('data' => $row);
  }
  // Allow modules to adjust table,
  $modules = module_implements('gradebook_adjust_grade_form_table');
  $table = array('header' => $header, 'rows' => $rows);
  foreach ($modules as $module) {
    $func = $module . '_gradebook_adjust_grade_form_table';
    $table = $func($form, $table);
  }

  $output = theme('table', $table['header'], $table['rows'], array('class' => 'gradebook'));
  $output .= theme('pager', NULL, variable_get('gradebook_grades_per_page', 10), 0);  // configure at admin screen!
  $output .= '<div>' . _gradebook_module_additions('bottom', $form, -1, $form['teacher']['#value']) . '</div>';
  $output .= '<div>' . drupal_render($form['submit']) . drupal_render($form['cancel']) . '</div>';
  $output .= drupal_render($form);
  return $output;
}

/**
 * Gathers HTML to add to the gradebook grade form from submodules
 * hook_gradebook_form_add_content.
 *
 * @param $position
 *   The position on the form to insert the content.
 * @param $form
 *   The form array, passed by reference.
 * @param $teacher
 *   TRUE if the current user is a teacher.
 * @param $separator
 *   HTML element to separate the content from each submodule.
 *
 * @return
 *   HTML to add to the grade form.
 */
function _gradebook_module_additions($position, &$form, $id, $teacher, $separator = '') {
  $output = '';
  $modules = module_implements('gradebook_form_add_content');
  foreach ($modules as $module) {
    $func = $module .'_gradebook_form_add_content';
    $addition = $func($position, $form, $id, $teacher);  // need to pass by reference!
    if ($addition != '') {
      $output .= $addition;
      $output .= $separator;
    }
  }
  return $output;
}

/**
 * Rebuilds the grades and returns the user to the gradebook page.
 *
 * @param $gradebook
 *   The gradebook object.
 *
 */
function gradebook_rebuild_page($gradebook) {
  gradebookapi_rebuild_grades($gradebook->tid);
  drupal_goto('gradebook/'. $gradebook->tid);
}
