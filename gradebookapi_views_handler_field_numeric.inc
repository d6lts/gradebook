<?php
/**
 * @file
 * Render a field as a numeric value only if it is not NULL.
 *
 * @ingroup views_field_handlers
 */
class gradebookapi_views_handler_field_numeric extends views_handler_field_numeric {
function option_definition() {
    $options = parent::option_definition();

    $options['null_text'] = array('default' => '');
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

      $form['null_text'] = array(
        '#type' => 'textfield',
        '#title' => t('Null text'),
        '#default_value' => $this->options['null_text'],
        '#description' => t('Text to replace ungraded (null) result.'),
        '#size' => 6,
      );
  }

  function render($values) {
    if (isset($values->{$this->field_alias})) {
      return parent::render($values);
    }
    else {
      return $this->options['null_text'];
    }
  }
}
