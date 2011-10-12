<?php
// $Id$

/**
 * @file
 *
 */

function auto_opengraph_settings_form() {

  $form['auto_opengraph_tags__default'] = _auto_opengraph_form(NULL, NULL, t('Default opengraph tag patterns'));
  $form['auto_opengraph_tags__front'] = _auto_opengraph_form('front', NULL, t('Overrides for front page'));
  $form['auto_opengraph_tags__front']['auto_opengraph_tags_render__front'] = array(
    '#type' => 'checkbox',
    '#title' => t('Output different tags for front page'),
    '#tree' => FALSE,
    '#default_value' => variable_get('auto_opengraph_tags_render__front', TRUE),
  );
  $form['auto_opengraph_write_description'] = array(
    '#type' => 'checkbox',
    '#title' => t('Also use og:description as meta description'),
    '#default_value' => variable_get('auto_opengraph_write_description', FALSE),
    '#description' => t('Useful for some search engines')
  );
  $form['auto_opengraph_style_images'] = array(
    '#type' => 'checkbox',
    '#title' => t('Style og:image according to Facebook specifications'),
    '#default_value' => variable_get('auto_opengraph_style_images', TRUE),
    '#description' => t('At least 50x50 pixels, max aspect ratio of 3:1')
  );

  $auto_opengraph_object_types = variable_get('auto_opengraph_entity_types', array('node', 'term'));
  $form['auto_opengraph_entity_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Auto opengraph object types:'),
    '#options' => array('node' => t('Nodes'), 'term' => t('Taxonomy terms'), 'user' => t('Users')),
    '#description' => t('Auto opengraph will manage og meta tags for all selected object types.'),
    '#default_value' => $auto_opengraph_object_types,
  );
  foreach ($auto_opengraph_object_types as $type) {
    $name = ucfirst($type);
    $form[$type] = array(
      '#type' => 'fieldset',
      '#title' => t($name),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form[$type]["auto_opengraph_tags__{$type}"] = _auto_opengraph_form($type, NULL, t("Overrides for all @types", array('@types' => "{$type}s")));

    // Form for each bundle
    switch($type) {
      case 'node':
        foreach (node_get_types('names') as $t => $n) {
          $form[$type]["auto_opengraph_tags__{$type}__{$t}"] = _auto_opengraph_form($type, $t, t('Overrides for @type nodes', array('@type' => $n)));
        }
        break;
      case 'term':
        foreach (taxonomy_get_vocabularies() as $vocabulary) {
          $form[$type]["auto_opengraph_tags__{$type}__{$vocabulary->vid}"] = _auto_opengraph_form($type, $vocabulary->vid, t('Overrides for @type vocabulary terms', array('@type' => $vocabulary->name)));
        }
        break;
    }

  }
  $form['array_filter'] = array('#type' => 'hidden');
  $form['#submit'] = array('auto_opengraph_form_preprocess');
  return system_settings_form($form);
}

function auto_opengraph_form_preprocess($form, &$form_state) {
  foreach($form_state['values'] as $field => $value) {
    if (stripos($field, 'auto_opengraph_tags__') === 0) {
      $val = $value['Basic'] + $value['Advanced'];
      variable_set($field, array_filter($val));
      unset($form_state['values'][$field]);
    }
  }
}