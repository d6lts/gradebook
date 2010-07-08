<?php

// $Id$

/**
 * @file
 * Gradebook Quiz Extension
 *
 * This file implements quiz hooks to integrate with gradebook.
 */

/**
 * Implements hook_quiz_finished().
 */

function gradebook_quiz_finished($quiz, $score, $rid) {
  // When Quiz is finished, create a gradebook grade.
  // Check to see if this is an allowed assignment type.
  $assignment_types = gradebookapi_get_assignment_types();
  if (isset($assignment_types['quiz'])) {
    // Discover the gradebooks associated with this node.
    $terms = gradebookapi_assignment_terms($quiz);
    $gids = array();
    foreach ($terms as $term) {
      $gradebook = gradebookapi_get_tid_gradebook($term->tid);
      $gids[] = $gradebook->tid;
    }
    if (!empty($gids)) {
      foreach (array_unique($gids) as $gid) {
        $gradebook = gradebookapi_get_tid_gradebook($gid);
        if (gradebookapi_is_student($gradebook)) {
          // User is a student completing an assignment.
          global $user;
          // Earned score is based on gradebook assignment possible.
          $grade = gradebookapi_get_grade($user->uid, $quiz->nid);
          $grade->earned = $score['numeric_score'];
          $grade->earned *= ($score['possible_score'] != 0) ? ($quiz->possible / $score['possible_score']) : 1;
          $grade->due_date = $quiz->due_date;
          $grade->publish_date = $quiz->publish_date;
          gradebookapi_set_grade($grade);
        }
      }
    }
  }
}
