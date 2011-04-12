<?php
/**
 * @file
 * Render the earned grade either as points or a percentage of possible.
 *
 * @ingroup views_field_handlers
 */
class gradebookapi_views_handler_field_earned extends gradebookapi_views_handler_field_numeric {
function option_definition() {
    $options = parent::option_definition();
    $options['percentage'] = array('default' => 0);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['percentage'] = array(
      '#type' => 'checkbox',
      '#title' => t('Show percentage instead of earned points'),
      '#default_value' => $this->options['percentage'],
      '#description' => t('Check this to display the percentage of possible points earned on this assignment, rather than the points earned.'),
      '#weight' => -100,
    );
  }

  function construct() {
    parent::construct();
    $this->additional_fields['possible'] = array(
        'table' => 'gradebookapi_assignment',
        'field' => 'possible',
    );
  }

  function render($values) {
    if ($this->options['percentage']) {
      if  (isset($values->{$this->aliases['possible']}) && $values->{$this->aliases['possible']} != 0) {
        $values->{$this->field_alias} = 100*$values->{$this->field_alias}/$values->{$this->aliases['possible']};
      }
    }

    return parent::render($values);
  }
}
