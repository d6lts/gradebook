<?php
// $Id$
/**
 * @file gradebookapi.rules.inc
 * Rules integration for the Gradebookapi module.
 */

// @TODO: Events that could be added.
// new assignment has been created for a class, or an existing assignment has been modified
//   -- might want to email class to let them know, or perform some other action.

/**
* Implementation of hook_rules_event_info().
* @ingroup rules
*/
function gradebookapi_rules_event_info() {
  return array(
    'gradebookapi_set_grade' => array(
      'label' => t('After assignment has been graded'),
      'module' => 'Gradebookapi',
      'arguments' => array(
        'user' => array('type' => 'user', 'label' => t('User that has received a grade.')),
        'node' => array('type' => 'node', 'label' => t('The assignment node that has been graded.')),
      ),
    ),
  );
}

// @TODO: Conditions that could be added
// assignment is current (date is before the due date, but after the published date)
// assignment has been graded

/**
 * Implementation of hook_rules_condition_info()
 */
function gradebookapi_rules_condition_info() {
  $items = array();
  $defaults = array(
    'arguments' => array(
      'node' => array('type' => 'node', 'label' => t('Content')),
    ),
    'module' => 'Gradebookapi',
  );
  $items['rules_condition_content_is_assignment'] = $defaults + array(
    'label' => t('Content is an assignment'),
    'help' => t('Evaluates to TRUE, if the given content is a gradebook assignment.'),
  );
  return $items;
}

/**
 * Condition: Check if node is an assignment node.
 */
function rules_condition_content_is_assignment($node) {
  return array('is_assignment' => gradebookapi_is_assignment_node($node));
}



/**
 * Implementation of hook_rules_action_info().
 */
function gradebookapi_rules_action_info() {
  return array(
    'rules_action_add_grade' => array(
      'label' => t('Add Grade'),
      'arguments' => array(
        'uid' => array('type' => 'number', 'label' => t('User ID')),
        'nid' => array('type' => 'number', 'label' => t('Assignment ID')),
        'earned' => array('type' => 'number', 'label' => t('Earned Points')),
        'exempt' => array(
          'type' => 'boolean',
          'label' => t('This assignment is not inluded in the overall grade for the student.'),
        ),
        'note' => array(
          'type' => 'string',
          'label' => t('Note'),
          'description' => t('A note explaining the grade to the student.'),
        ),
      ),
      'new variables' => array(
        'node_added' => array(
          'type' => 'grade',
          'label' => t('New Grade'),
          'save' => TRUE,
        ),
      ),
      'module' => 'Gradebookapi',
    ),
    'rules_action_load_grade' => array(
      'label' => t('Load grade by user id and assignment id'),
      'arguments' => array(
        'uid' => array('type' => 'number', 'label' => t('User ID')),
        'nid' => array('type' => 'number', 'label' => t('Assignment ID')),
      ),
      'new variables' => array(
        'grade_loaded' => array(
          'type' => 'grade',
          'label' => t('Loaded Grade'),
        ),
      ),
      'module' => 'Gradebookapi',
    ),
    'rules_action_get_assignment_possible' => array(
      'label' => t('Get possible points for this assignment node'),
      'arguments' => array(
        'node' => array('type' => 'node', 'label' => t('Assignment node')),
      ),
      'new variables' => array(
        'points_possible' => array(
          'type' => 'number',
          'label' => t('Points Possible'),
        ),
      ),
      'module' => 'Gradebookapi',
    ),
    'rules_action_is_assignment' => array(
      'label' => t('Get possible points for this assignment node'),
      'arguments' => array(
        'node' => array('type' => 'node', 'label' => t('Assignment node')),
      ),
      'new variables' => array(
        'points_possible' => array(
          'type' => 'number',
          'label' => t('Points Possible'),
        ),
      ),
      'module' => 'Gradebookapi',
    ),
  );
}

/**
 * Action "Add a grade"
 */
function rules_action_add_grade($uid, $nid, $earned, $exempt, $note) {
  $grade = (object)array('uid' => $uid, 'nid' => $nid, 'earned' => $earned, 'exempt' => $exempt, 'note' => $note);
  return array('grade_added' => $grade);
}

/**
 * Loads a Grade
 */
function rules_action_load_grade($uid, $nid) {
  return array('grade_loaded' => gradebookapi_get_grade($uid, $nid));
}

/**
 * Gets possible points from an assignment node
 */
function rules_action_get_assignment_possible($node) {
  return array('points_possible' => isset($node->possible) ? $node->possible : 0);
}


/**
* Implementation of hook_rules_data_type_info().
*/
function gradebookapi_rules_data_type_info() {
  return array(
    'grade' => array(
      'label' => t('Grade'),
      'class' => 'rules_data_type_grade',
      'savable' => TRUE,
      'identifiable' => TRUE,
      'uses_input_form' => FALSE,
      'module' => 'Gradebookapi',
    ),
  );
}

/**
* Defines the rules grade data type.
*/
class rules_data_type_grade extends rules_data_type {

  function save() {
    $grade = &$this->get();
    gradebookapi_set_grade($grade);
    return TRUE;
  }

  function load($identifier) {
    return gradebookapi_get_grade($identifier['uid'], $identifier['nid']);
  }

  function get_identifier() {
    $grade = &$this->get();
    return array('uid' => $grade->uid, 'nid' => $grade->nid);
  }
}
