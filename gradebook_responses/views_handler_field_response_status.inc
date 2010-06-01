<?php
// $Id$

/**
 * A handler to provide proper displays for Gradebook Response Status.
 *
 * @ingroup views_field_handlers
 */
class views_handler_field_response_status extends views_handler_field {
  function render($values) {
    $value = $values->{$this->field_alias};
    $status_string = _gradebook_responses_status_string();
    return $status_string[$value];
  }
}