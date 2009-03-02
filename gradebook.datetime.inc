<?php
// $Id$
/**
 * @file
 * Helper functions for gradebook dates and times
 */

function time_field_elements() {
  $type['time_field_time'] = array(
    '#input' => TRUE,
    '#process' => array('time_field_expand_time'),
  );
  return $type;
}

function time_field_expand_time($element) {
  if (empty($element['#value'])) {
    $element['#value'] = array(
      'hour' => intval(format_date(time(), 'custom', 'h')),
      'minute' => intval(format_date(time(), 'custom', 'i')),
      'meridiem' => format_date(time(), 'custom', 'a'),
    );
  }

  $element['#tree'] = TRUE;

  foreach ($element['#value'] as $type => $value) {
    switch ($type) {
      case 'hour':
        $options = drupal_map_assoc(range(1, 12));
        break;
      case 'minute':
        $options = drupal_map_assoc(range(0, 59));
        break;
      case 'meridiem':
        $options = drupal_map_assoc(array('am', 'pm'));
        break;
    }

    if ($type == 'hour' || $type == 'minute') {
      foreach ($options as $option) {
        $options[$option] = str_pad($options[$option], 2, '0', STR_PAD_LEFT);
      }
    }
    $parents = $element['#parents'];
    $parents[] = $type;
    $element[$type] = array(
      '#type' => 'select',
      '#default_value' => $element['#value'][$type],
      '#attributes' => $element['#attributes'],
      '#options' => $options,
    );
  }

  return $element;
}

/**
 * Implementation of hook_theme().
 */
function time_field_theme($existing, $type, $theme, $path) {
  return array(
    'time_field_time' => array(
      'arguments' => array('element' => NULL),
    ),
  );
}

function theme_time_field_time($element) {
  return theme('form_element', $element, '<div class="container-inline">'. $element['#children'] .'</div>');
}


/**
 *  These date functions are copied and adapted from the announcements module
 *  TODO: document these functions
 */


function _gradebookapi_get_timezone() {
  global $user;
  if (variable_get('configurable_timezones', 1) && $user->uid && strlen($user->timezone)) {
    // User has his/her own timezone, use that.
    return $user->timezone;
  }
  return variable_get('date_default_timezone', 0);
}

function _gradebookapi_drupaldate2unixtime($drupal_date) {
  // Compute the time in UTC (GMT).
  $tz_seconds = _gradebookapi_get_timezone();
  return gmmktime(0, 0, 0, (int)$drupal_date["month"], (int)$drupal_date["day"], (int)$drupal_date["year"]) - $tz_seconds;
}

function _gradebookapi_unixtime2drupaldate($unixtime) {
  $unixtime -= _gradebookapi_get_timezone();
  return array('day' => gmdate('j', $unixtime),
    'month' => gmdate('n', $unixtime),
    'year' => gmdate('Y', $unixtime));
}

function _gradebookapi_datepopup2unixtime($popup_date) {
  // TODO: Consider timezone for popup? If so, what does the form element need to be to make it work?
  return strtotime($popup_date);
}

function _gradebookapi_unixtime2datepopup($unixtime) {
// TODO: Consider timezone for popup? If so, what does the form element need to be to make it work?
  return date("Y-m-d H:i", $unixtime);
}
