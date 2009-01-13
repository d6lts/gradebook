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

function gradebook_grade_page($gradebook, $uid, $nid) {
  $account = user_load(array('uid' => $uid));
  if ($account !== FALSE && gradebookapi_is_student($gradebook, $account)) {
    $types = gradebookapi_get_assignment_types();
    $node = node_load($nid);
    if (in_array($node->type, $types)) {
      $grade = gradebookapi_get_grade($uid, $nid);  //now its safe to get $grade

      // check if this is a student or a teacher.

      if (gradebookapi_is_teacher($gradebook)) { // a teacher can edit the grade
        return drupal_get_form('gradebook_grade_form', $gradebook, $account, $node, (array)$grade);
      }
      else {  // must be a student - access callback ensures this - show a grade summary for their grades only
        global $user;
        $current = user_load(array('uid' => $user->uid));   // want to change url if $current != account?
        return theme('gradebook_assignment_note', $gradebook->tid, $current, $node, $grade);
      }
    }
  }
  drupal_not_found();
}

function gradebook_grade_form($form_state, $gradebook, $account, $node, $edit = array()) {

  $form['user'] = array(
    '#prefix' => '<div class="gradebook-grade-student-form">',
    '#value' => t('Student: %name', array('%name' => $account->name)),
    '#suffix' => '</div>',
  );

  $form['node'] = array(
    '#prefix' => '<div class="gradebook-grade-title-form">',
    '#value' => t('Assignment: %title', array('%title' => $node->title)),
    '#suffix' => '</div>',
  );

  $form['possible'] = array(
    '#prefix' => '<div class="gradebook-grade-possible-form">',
    '#value' => t('Possible: %possible', array('%possible' => $node->possible)),
    '#suffix' => '</div>',
  );

  $form['earned'] = array(
    '#title' => t('Earned'),
    '#type' => 'textfield',
    '#prefix' => '<div class="gradebook-grade-earned-form">',
    '#default_value' => $edit['earned'],
    '#maxlength' =>  64,
    '#description' => t('The grade earned.'),
    '#suffix' => '</div>',
    '#required' => TRUE
  );

  $form['exempt'] = array(
    '#type' => 'checkbox',
    '#prefix' => '<div class="gradebook-grade-exempt-form">',
    '#title' => t('Exempt student from this assignment'),
    '#suffix' => '</div>',
    '#default_value' => $edit['exempt'],
  );

  $form['note'] = array(
    '#type' => 'textarea',
    '#prefix' => '<div class="gradebook-grade-note-form">',
    '#title' => t('Note to student'),
    '#suffix' => '</div>',
    '#default_value' => $edit['note'],
  );

  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => $account->uid,
  );
  $form['nid'] = array(
    '#type' => 'hidden',
    '#value' => $node->nid,
  );

  $form['return_url'] = array(
   '#type' => 'hidden',
   '#value' => 'gradebook/'. $gradebook->tid,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function gradebook_grade_form_validate($form, &$form_state) {
  if (is_numeric($form_state['values']['earned']) ? intval($form_state['values']['earned']) != $form_state['values']['earned'] : TRUE) {
    form_set_error('earned', t('The earned value for the assignment must be an integer.'));
  }
}

function gradebook_grade_form_submit($form, &$form_state) {
  $account = user_load(array('uid' => $form_state['values']['uid']));
  $node = node_load($form_state['values']['nid']);

  $grade = (object)$form_state['values'];
  $grade->possible = $node->possible;

  gradebookapi_set_grade($grade);
  drupal_set_message(t('Saved %type.', array('%type' => t('grade'))));
  $form_state['redirect'] = $form_state['values']['return_url'];
  unset($form_state['values']['return_url']);
}

function gradebook_rebuild_page($gradebook) {
  gradebookapi_rebuild_grades($gradebook->tid);
  drupal_goto('gradebook/'. $gradebook->tid);
}

function theme_gradebook_assignment_note($tid, $account, $node, $grade) {
  $header = array();
  $row = array();
  $row[] = array(t('Student'), $account->name);
  $row[] = array(t('Assignment'), $node->title);
  $row[] = array(t('Possible'), $node->possible);
  $row[] = array(t('Earned'), $grade->earned);
  $row[] = array(t('Note'), $grade->note);
  $row[] = array('', l(t('Return to gradebook'), 'gradebook/'. $tid));
  return theme('table', $header, $row);
}

//
// MGN: working on extending the UI for entering grades by assignment
//

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
  //  $param = array('a.nid' => $nid);
  //  $grades = gradebookapi_get_grades($param);  // get all student grades for this assignment
  // check if g.nid exists, if so, do the select, if not make up an empty grade object
    $sql = 'SELECT uid, nid, earned, exempt, timestamp, note FROM {gradebookapi_grade} WHERE nid = %d';
    $result = db_query($sql, $nid);
    $grades = array();
    while ($grade = db_fetch_object($result)) {
      $grades[$grade->uid] = (array)$grade;
    }
    $students = gradebookapi_get_students($gradebook);
    foreach ($students as $student) {
      if (!isset($grades[$student])) {  // create an empty record
        $grades[$student] = array('uid' => $student, 'nid' => $nid, 'earned' => NULL, 'exempt' => NULL, 'timestamp' => NULL, 'note' => NULL,);  // only for form building - later unset any grade NULL 'earned' field.
      }
    }
    return drupal_get_form('gradebook_grade_assignment_form', $gradebook, $node, $grades);
  }
  drupal_not_found();
}

function gradebook_grade_assignment_form($form_state, $gradebook, $node, $edit=array()) {
  // TODO: cleanup grade form rendering
  //       ideally edit is in-place in gradebook using AJAX
  //  fields uid, nid, earned, exempt, possible, timestamp, note
  // loop over students

  $form['node'] = array(
    '#prefix' => '<div class="gradebook-grade-title-form">',
    '#value' => t('Assignment: %title', array('%title' => $node->title)),
    '#suffix' => '</div>',
  );

  $form['possible'] = array(
    '#prefix' => '<div class="gradebook-grade-possible-form">',
    '#value' => t('Possible: %possible', array('%possible' => $node->possible)),
    '#suffix' => '</div>',
  );

  $form['grade'] = array (
    '#tree' => TRUE,
  );    

  foreach ($edit as $grade) {
    $grade = (array)$grade;
    $uid = $grade['uid'];
    $account = user_load(array('uid' => $uid));

    $form['grade'][$uid] = array(
      '#prefix' => '<div class="gradebook-grade-student-form">',
      '#value' => t('Name: %name', array('%name' => $account->name)),
      '#suffix' => '</div>',
      '#tree' => TRUE,
     ); 

    $form['grade'][$uid]['exempt'] = array(
      '#type' => 'checkbox',
      '#prefix' => '<div class="gradebook-grade-exempt-form">',
      '#title' => t('Exempt'),
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#default_value' => $grade['exempt'],
    );

    $form['grade'][$uid]['earned'] = array(
      '#title' => t('earned'),
      '#type' => 'textfield',
      '#prefix' => '<div class="gradebook-grade-earned-form">',
      '#default_value' => $grade['earned'],
      '#size' => 10,
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );

    $form['grade'][$uid]['note'] = array(
      '#type' => 'textarea',
      '#prefix' => '<div class="gradebook-grade-note-form">',
      '#title' => t('note'),
      '#default_value' => $grade['note'],
      '#cols' => 32,
      '#rows' => 5,
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );
  }

  $form['nid'] = array( '#type' => 'hidden', '#value' => $node->nid);
  $form['return_url'] = array( '#type' => 'hidden', '#value' => 'gradebook/'. $gradebook->tid);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

function gradebook_grade_assignment_form_validate($form, &$form_state) {
  $keys = array_keys($form_state['values']['grade']);
  foreach ($keys as $key) {
    if ($form_state['values']['grade'][$key]['earned'] != '' ) {
      $earned = $form_state['values']['grade'][$key]['earned'];
      if (is_numeric($earned) ? intval($earned) != $earned : TRUE) {
        form_set_error('grade]['.$key.'][earned', t('The earned value for the assignment must be an integer.'));
      }
    } 
    elseif ($form_state['values']['grade'][$key]['note'] != '' ) {
      form_set_error('grade]['.$key.'][earned', t('A grade must be given before the note can be saved.'));
    }
  }
}

function gradebook_grade_assignment_form_submit($form, &$form_state) {
  $node = node_load($form_state['values']['nid']);

  $keys = array_keys($form_state['values']['grade']);
  $count = 0;
  foreach ($keys as $key) {
    if ($form_state['values']['grade'][$key]['earned'] != '' ) { //only update when grades are provided...
      $grade = (object)$form_state['values']['grade'][$key];
      $grade->uid = $key;
      $grade->nid = $node->nid;
      $grade->possible = $node->possible;
      $grade->due_date = $node->due_date;
      $grade->publish_date = $node->publish_date;
      $grade->note = check_plain($grade->note);    // is this correct here?
      gradebookapi_set_grade($grade);
      ++$count;
    }
  }
  drupal_set_message(t('Saved %num %type.', array('%num' => $count, '%type' => t('grades'))));
  $form_state['redirect'] = $form_state['values']['return_url'];
  unset($form_state['values']['return_url']);
}

/* 

function theme_gradebook_grade_assignment_form($form) {
  $output = '';
  $output .= drupal_render($form['name']);
  $output .= '<div class="foo">';
  $output .= drupal_render($form['access']);
  $output .= '<div class="bar">';
  $output .= drupal_render($form['details']);
  $output .= '</div></div>';
  $output .= drupal_render($form);
  return $output;
}

*/

//
//  Edit all assignments for a selected student
//


/**
 *  Enter grades for all assignments for a selected student
 *    This form is only accessible to teachers
 *  @param $gradebook
 *    the $gradebook object
 *  @param $uid
 *    path argument for the student user id that we want to grade
 *
 */

function gradebook_grade_student_page($gradebook, $uid) {  // access is limited to teacher through menu system

  // get sort order for query
  $sort = (isset($_GET['sort']) && ($_GET['sort'] == 'asc')) ? 'desc' : 'asc'; 

  $account = user_load(array('uid' => $uid));
  if (gradebookapi_is_student($gradebook, $account)) {
    // get assignments
    $tids = array();
    $assignments = gradebook_get_sorted_assignments($gradebook, $tids, $order, $sort);

    // get grades for assignments
    $grades = array(); 
    foreach ($assignments as $assignment) {
      $grades[$assignment->nid] = (array)gradebookapi_get_grade($account->uid, $assignment->nid);
    }
    return drupal_get_form('gradebook_grade_student_form', $gradebook, $account, $grades);
  }
  drupal_not_found();
}

function gradebook_grade_student_form($form_state, $gradebook, $account, $edit=array()) {
  // TODO: cleanup grade form rendering
  //       ideally edit is in-place in gradebook using AJAX
  //  fields uid, nid, earned, exempt, possible, timestamp, note
  // loop over students

  $form['name'] = array(
      '#prefix' => '<div class="gradebook-grade-student-form">',
      '#value' => t('Name: %name', array('%name' => $account->name)),
      '#suffix' => '</div>',
  );

  $form['grade'] = array (
    '#tree' => TRUE,
  );    

  foreach ($edit as $grade) {
    $grade = (array)$grade;
    $nid = $grade['nid'];
    $node = node_load(array('nid' => $nid));

    $form['grade'][$nid] = array(
      '#prefix' => '<div class="gradebook-grade-title-form">',
      '#value' => t('Assignment: %name', array('%name' => $node->title)),
      '#suffix' => '</div>',
      '#tree' => TRUE,
     ); 

    $form['grade'][$nid]['possible'] = array(
      '#prefix' => '<div class="gradebook-grade-possible-form">',
      '#value' => t('Possible: %possible', array('%possible' => $node->possible)),
      '#tree' => TRUE,
      '#suffix' => '</div>',
    );

    $form['grade'][$nid]['exempt'] = array(
      '#type' => 'checkbox',
      '#prefix' => '<div class="gradebook-grade-exempt-form">',
      '#title' => t('Exempt'),
      '#suffix' => '</div>',
      '#tree' => TRUE,
      '#default_value' => $grade['exempt'],
    );

    $form['grade'][$nid]['earned'] = array(
      '#prefix' => '<div class="gradebook-grade-earned-form">',
      '#title' => t('earned'),
      '#type' => 'textfield',
      '#default_value' => $grade['earned'],
      '#size' => 10,
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );

    $form['grade'][$nid]['note'] = array(
      '#type' => 'textarea',
      '#prefix' => '<div class="gradebook-grade-note-form">',
      '#title' => t('note'),
      '#default_value' => $grade['note'],
      '#cols' => 32,
      '#rows' => 5,
      '#suffix' => '</div>',
      '#tree' => TRUE,
    );
  }

  $form['uid'] = array( '#type' => 'hidden', '#value' => $account->uid);
  $form['return_url'] = array( '#type' => 'hidden', '#value' => 'gradebook/'. $gradebook->tid);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

// this is a duplication of gradebook_grade_assignment_form_validate
// how can we share form validation?

function gradebook_grade_student_form_validate($form, &$form_state) {
  $keys = array_keys($form_state['values']['grade']);
  foreach ($keys as $key) {
    if ($form_state['values']['grade'][$key]['earned'] != '' ) {
      $earned = $form_state['values']['grade'][$key]['earned'];
      if (is_numeric($earned) ? intval($earned) != $earned : TRUE) {
        form_set_error('grade]['.$key.'][earned', t('The earned value for the assignment must be an integer.'));
      }
    } 
    elseif ($form_state['values']['grade'][$key]['note'] != '' ) {
      form_set_error('grade]['.$key.'][earned', t('A grade must be given before the note can be saved.'));
    }
  }
}

function gradebook_grade_student_form_submit($form, &$form_state) {

  $nids = array_keys($form_state['values']['grade']);
  $count = 0;
  foreach ($nids as $nid) {
    if ($form_state['values']['grade'][$nid]['earned'] != '' ) { //only update when grades are provided...
      $node = node_load($nid);  // check syntax here
      $grade = (object)$form_state['values']['grade'][$nid];
      $grade->nid = $nid;
      $grade->uid = $form_state['values']['uid'];
      $grade->possible = $node->possible;
      $grade->due_date = $node->due_date;
      $grade->publish_date = $node->publish_date;
      $grade->note = check_plain($grade->note);    // is this correct here?
      gradebookapi_set_grade($grade);
      ++$count;
    }
  }
  drupal_set_message(t('Saved %num %type.', array('%num' => $count, '%type' => t('grades'))));
  $form_state['redirect'] = $form_state['values']['return_url'];
  unset($form_state['values']['return_url']);
}

/* 

function theme_gradebook_grade_student_form($form) {
  $output = '';
  $output .= drupal_render($form['name']);
  $output .= '<div class="foo">';
  $output .= drupal_render($form['access']);
  $output .= '<div class="bar">';
  $output .= drupal_render($form['details']);
  $output .= '</div></div>';
  $output .= drupal_render($form);
  return $output;
}
*/

