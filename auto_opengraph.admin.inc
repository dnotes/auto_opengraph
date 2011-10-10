<?php
// $Id$

/**
 * @file
 *
 */

function auto_opengraph_settings_form() {
  
  $form['auto_opengraph_tags_default'] = _auto_opengraph_form(NULL, NULL, t('Default opengraph tag patterns'));
  $form['auto_opengraph_tags_front'] = _auto_opengraph_form('front', NULL, t('Overrides for front page'));

  // TODO: add custom og:types
  // TODO: add app namespace

  // TODO: add custom fields?

  $entity_types = _auto_opengraph_map_entities();
  $auto_opengraph_entity_types = variable_get('auto_opengraph_entity_types', array('node', 'taxonomy_term'));
  $form['auto_opengraph_entity_types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Auto opengraph entity types:'),
    '#options' => array_combine(array_keys($entity_types), array_keys($entity_types)),
    '#description' => t('Auto opengraph will manage og meta tags for all selected entity types.'),
    '#default_value' => $auto_opengraph_entity_types,
  );
  foreach ($auto_opengraph_entity_types as $type) {
    $name = ucfirst(str_replace('_', ' ', $type));
    $form[$type] = array(
      '#type' => 'fieldset',
      '#title' => t($name),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form[$type]["auto_opengraph_tags-$type"] = _auto_opengraph_form($type, NULL, t("Overrides for all $type entities"));

    // Form for each bundle
    $info = entity_get_info($type);
    if (count($info['bundles']) > 1) {
      foreach (element_children($info['bundles']) as $bundle) {
        $form[$type]["auto_opengraph_tags-$type:$bundle"] = _auto_opengraph_form($type, $bundle, t("Overrides for $type:$bundle"));
      }
    }

  }
  return $form;
}
