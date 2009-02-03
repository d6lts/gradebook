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
    foreach ($students as $uid) {
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

  $attach_allowed = array_filter((array)variable_get('gradebook_attachment_roles', array()));
  $attach_permission = (gradebookapi_is_student($gradebook) && in_array('students', $attach_allowed)) || ($teacher && in_array('teachers', $attach_allowed)) || user_access('attach files to gradebook');

  $form = array('#theme' => 'gradebook_grade_form');
  $form['grade'] = array('#tree' => TRUE);

  $options = array();
  foreach ($grades as $id => $grade) {
    $options[$id] = '';  // radio buttons for each grade, but no labels
    $account = user_load(array('uid' => $grade['uid']));
    $node = node_load($grade['nid']);
    $form['grade'][$id]['uid'] = array('#type' => 'hidden', '#value' => $account->uid);
    $form['grade'][$id]['nid'] = array('#type' => 'hidden', '#value' => $node->nid);
    $form['grade'][$id]['user'] = array('#value' => $account->name);
    $form['grade'][$id]['node'] = array('#value' => $node->title);
    $form['grade'][$id]['possible'] = array('#value' => $node->possible);

    if ($teacher) {  // display edit form
      $form['grade'][$id]['earned'] = array('#type' => 'textfield', '#size' => 2, '#default_value' => $grade['earned']);
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#default_value' => $grade['exempt']);
      $form['grade'][$id]['note'] = array('#type' => 'textarea', '#cols' => 25, '#rows' => 6, '#default_value' => $grade['note']);
    }
    else { // display grades without option for editing
      $form['grade'][$id]['earned'] = array('#value' => $grade['earned']);
      $form['grade'][$id]['exempt'] = array('#type' => 'checkbox', '#value' => $grade['exempt'], '#disabled' => TRUE);
      $form['grade'][$id]['note']   = array('#value' => $grade['note']);
    }

    if (!empty($form_state['storage'][$id])) {
      foreach ($form_state['storage'][$id] as $file) {
        $grade['files'][$file['fid']] = $file;
      }
    }
    $form['grade'][$id]['attachments'] = array(
      '#prefix' => '<div id="gradebook-attachments">',
      '#suffix' => '</div>',
    );
    $form['grade'][$id]['attachments'] += gradebook_upload_form($id, $grade, $teacher);
  }
  if ($attach_permission) {
    // Make sure necessary directories exist and are writable before displaying the attachment form.
    $path = file_directory_path();
    $temp = file_directory_temp();
    if (!file_check_directory($path, FILE_CREATE_DIRECTORY) || !file_check_directory($temp, FILE_CREATE_DIRECTORY)) {
      $form['new']['#description'] =  t('File attachments are disabled. The file directories have not been properly configured.');
      if (user_access('administer site configuration')) {
        $form['new']['#description'] .= ' '. t('Please visit the <a href="@admin-file-system">file system configuration page</a>.', array('@admin-file-system' => url('admin/settings/file-system')));
      }
      else {
        $form['new']['#description'] .= ' '. t('Please contact the site administrator.');
      }
    }
    $limits = _upload_file_limits($user);
    $form['new']['#weight'] = 10;
    $form['new']['upload'] = array(
      '#type' => 'file',
      '#title' => t('Attach new file'),
      '#size' => 40,
      '#description' => t('Select a grade to attach file to before pressing <em>attach</em>. Files will not be saved until the the <em>Save All</em> button is pressed.') .  ($limits['resolution'] ? t('Images are larger than %resolution will be resized. ', array('%resolution' => $limits['resolution'])) : '') . t('The maximum upload size is %filesize. Only files with the following extensions may be uploaded: %extensions. ', array('%extensions' => $limits['extensions'], '%filesize' => format_size($limits['file_size']))),
    );
    $form['new']['attach'] = array('#type' => 'submit', '#value' => t('Attach'), '#validate' => array('gradebook_upload_form_attach_another'));
    $form['#attributes']['enctype'] = 'multipart/form-data';
    $form['attach permission'] = array('#type' => 'value', '#value' => TRUE);
  }
  else {
    $form['attach permission'] = array('#type' => 'value', '#value' => FALSE);
  }
  if (count($options) > 1) {
    $form['gradeid'] = array('#type' => 'radios', '#required' => FALSE, '#options' => $options);
  }
  else {
    $form['gradeid'] = array('#type' => 'hidden', '#value' => $id);  // automatically selected
  }

  $form['return_url'] = array('#type' => 'hidden', '#value' => 'gradebook/'. $gradebook->tid);
  $form['teacher'] = array('#type' => 'hidden', '#value' => $teacher);
  if ($teacher || $attach_permission) {  //teachers might be saving grades, students may attach files
    $form['submit'] = array('#type' => 'submit', '#value' => t('Save All'));
  }
  $form['cancel'] = array('#type' => 'submit', '#value' =>  t('Return to Gradebook'), '#submit' => array('gradebook_grade_form_cancel'));

  return $form;
}
function gradebook_upload_form($id, $edit, $teacher = FALSE) {

  $form = array('#cache' => TRUE);

  if (isset($edit['files'])) {
    $files = $edit['files'];
  }

  if (!empty($files) && is_array($files)) {  // need to show files that are already uploaded...
    $form['files']['#tree'] = TRUE;
    global $user;
    foreach ($files as $key => $file) {
      $file = (object)$file;
      $allow_edit = $teacher  || ($user->uid == $file->uid);  // users can edit their own uploads, teachers can edit all
      if ($file->list || $allow_edit) { // display the form elements
        $form['files'][$key]['description'] = array('#type' => 'textfield', '#default_value' => !empty($file->description) ? $file->description : $file->filename, '#size' => 25, '#maxlength' => 255, '#disabled' => !$allow_edit);        
        $form['files'][$key]['remove'] = array('#type' => 'checkbox', '#default_value' => !empty($file->remove), '#disabled' => !$allow_edit);
        $form['files'][$key]['list'] = array('#type' => 'checkbox',  '#default_value' => $file->list, '#disabled' => !$allow_edit);
      }
      else {
        $form['files'][$key]['description'] = array('#type' => 'value', '#value' => !empty($file->description) ? $file->description : $file->filename);
        $form['files'][$key]['remove'] = array('#type' => 'value', '#value' => !empty($file->remove));
        $form['files'][$key]['list'] = array('#type' => 'value', '#value' => $file->list);
      }
      $form['files'][$key]['size']     = array('#type' => 'value', '#value' => format_size($file->filesize));
      $form['files'][$key]['filename'] = array('#type' => 'value', '#value' => $file->filename);
      $form['files'][$key]['filepath'] = array('#type' => 'value', '#value' => $file->filepath);
      $form['files'][$key]['filemime'] = array('#type' => 'value', '#value' => $file->filemime);
      $form['files'][$key]['filesize'] = array('#type' => 'value', '#value' => $file->filesize);
      $form['files'][$key]['fid']      = array('#type' => 'value', '#value' => $file->fid);
      $form['files'][$key]['uid']      = array('#type' => 'value', '#value' => $file->uid);
      $form['files'][$key]['saved']    = array('#type' => 'value', '#value' => $file->saved);
    }
  }

  return $form;
}

function gradebook_upload_form_attach_another($form, &$form_state) {
  // handle the file upload

  global $user;

  $limits = _upload_file_limits($user);
  $validators = array(
    'file_validate_extensions' => array($limits['extensions']),
    'file_validate_image_resolution' => array($limits['resolution']),
    'file_validate_size' => array($limits['file_size'], $limits['user_size']),
  );
  // Save new file uploads.
  if ($form_state['values']['attach permission'] && ($file = file_save_upload('upload', $validators, file_directory_path()))) {
    $id = $form_state['values']['gradeid'];
    $file->list = 1; // list files by default
    $file->saved = FALSE;
    $file->uid = $user->uid;
    $file->description = $file->filename;
    $form_state['storage'][$id][$file->fid] = (array)$file;
  }
  $form_state['rebuild'] = TRUE;
}

/* 
 * No validation is needed for floating point grades, but saving this function for future development.
 */
function gradebook_grade_form_validate($form, &$form_state) {
// do nothing
}

function gradebook_grade_form_cancel($form, &$form_state) {
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
        gradebookapi_set_grade($grade);
      }
      else {
        gradebookapi_manage_attachments($grade); // must have permissions to upload files to get this far...
      }
      ++$count;
    }
    unset($form_state['storage']);  // clear the storage bins now that files have been saved - needed for redirect
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
  $ids = element_children($form['grade']);  // same as element_children ($form['gradeid'])
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
    $row[] = array('data' => drupal_render($form['grade'][$id]['node']), 'class' => 'title');
    $row[] = array('data' => drupal_render($form['grade'][$id]['earned']) .'/'. drupal_render($form['grade'][$id]['possible']), 'class' => 'earned-possible');
    $row[] = array('data' => drupal_render($form['grade'][$id]['exempt']), 'class' => 'exempt-box');
    $output = '<div class="note">'. drupal_render($form['grade'][$id]['note']) .'</div>';

    $keys = element_children($form['grade'][$id]['attachments']['files']);

    if (count($keys) > 0) {  // show attachment table if files are attached
      $attachment_header = array(t('Attachment'), t('Link'), t('List'), t('Remove'));
      $attachment_rows = NULL;
      foreach ($keys as $key) {
        $view_row = $form['grade'][$id]['attachments']['files'][$key]['list']['#value'] ||
                   ($form['grade'][$id]['attachments']['files'][$key]['uid']['#value'] == $user->uid) ||
                    $form['teacher']['#value'];  // allow teacher to view row, whatever the student checks...
        if ($view_row) {
          $attachment_row = NULL;
          $attachment_row[] = drupal_render($form['grade'][$id]['attachments']['files'][$key]['description']);
          $attachment_row[] = l('View', $form['grade'][$id]['attachments']['files'][$key]['filepath']['#value']);
          $attachment_row[] = drupal_render($form['grade'][$id]['attachments']['files'][$key]['list']);
          $attachment_row[] = drupal_render($form['grade'][$id]['attachments']['files'][$key]['remove']);
          $attachment_rows[] = array('data' => $attachment_row);
        }
      }
      $output .= (count($attachment_rows) > 0) ? theme('table', $attachment_header, $attachment_rows) : '';
    }
    $row[] = array('data' => $output);
    $rows[] = array('data' => $row);
  }
  $output = theme('table', $header, $rows, array('class' => 'gradebook'));
  $output .= theme('pager', NULL, variable_get('gradebook_grades_per_page', 10), 0);  // configure at admin screen!
  $output .= '<div>'. drupal_render($form['new']['upload']) . drupal_render($form['new']['attach']) .'</div>';
  $output .= '<div>'. drupal_render($form['submit']) . drupal_render($form['cancel']) .'</div>';
  $output .= drupal_render($form);
  return $output;
}

function gradebook_rebuild_page($gradebook) {
  gradebookapi_rebuild_grades($gradebook->tid);
  drupal_goto('gradebook/'. $gradebook->tid);
}
