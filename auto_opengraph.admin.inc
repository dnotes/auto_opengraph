<?php
// $Id$

/**
 * @file
 *
 */

function auto_opengraph_settings_form() {

  include_once 'auto_opengraph.fields.inc';

  $all_properties = auto_opengraph_properties();
  foreach ($all_properties as $property => $section) { $og[$property] = array(); }

  foreach (array('defaults', 'facebook', 'node', 'user', 'taxonomy', 'other') as $part) {
    if (function_exists($function = "_auto_opengraph_settings_form_$part")) {
      $form[$part] = call_user_func($function, $og);
    }
  }

  return system_settings_form($form);
}

function _auto_opengraph_settings_form_defaults() {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Defaults'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['auto_opengraph_default_image'] = array(
    '#type' => 'textfield', // TODO: add support for image upload.
    '#title' => t('Image'),
    '#description' => t('path to an image within the files folder'),
    '#default_value' => variable_get('auto_opengraph_default_image', ''),
  );
  foreach(imagecache_presets() as $preset) {
    $presets[$preset['presetname']] = $preset['presetname'];
  }
  array_unshift($presets, t('none (NOT recommended)'));
  $form['auto_opengraph_imagecache_preset'] = array(
    '#type' => 'select',
    '#title' => t('Imagecache profile for opengraph images'),
    '#options' => $presets,
    '#default_value' => variable_get('auto_opengraph_imagecache_preset', 0),
  );
  $form['auto_opengraph_default_description'] = array(
    '#type' => 'textarea',
    '#title' => t('Description'),
    '#description' => t('optional one or two sentence statement describing a page'),
    '#default_value' => variable_get('auto_opengraph_default_description', ''),
    '#rows' => 2,
  );
  $form['location'] = array(
    '#type' => 'fieldset',
    '#title' => t('Location'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('The default location for the site will be rendered on the home page of the site.<br/>
                        You may choose to render the location on node types, taxonomy pages, or other pages,<br/>
                        and the default location may be used there unless it is overridden by other fields.')
  );
  foreach (array('latitude', 'longitude', 'street-address', 'locality', 'region', 'postal-code', 'country-name') as $property) {
    $form['location']["auto_opengraph_default_$property"] = array(
      '#type' => 'textfield',
      '#title' => str_replace('-', ' ', $property),
      '#default_value' => variable_get("auto_opengraph_default_$property", ''),
    );
  }
  $form['contact'] = array(
    '#type' => 'fieldset',
    '#title' => t('Contact Information'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('The default contact information will be rendered on the home page of the site.<br/>
                        You may choose to render contact information on node types, taxonomy pages, or other pages,<br/>
                        and the defaults may be used there unless they are overridden by other fields.')
  );
  foreach (array('email', 'phone_number', 'fax_number') as $property) {
    $form['contact']["auto_opengraph_default_$property"] = array(
      '#type' => 'textfield',
      '#title' => str_replace('_', ' ', $property),
      '#default_value' => variable_get("auto_opengraph_default_$property", ''),
    );
  }
  $form['facebook'] = array(
    '#type' => 'fieldset',
    '#title' => t('Facebook Administrators'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Any page with fb:admins OR fb:app_id, along with all of the basic opengraph fields, will have a full Facebook presence.')
  );
  $form['facebook']["auto_opengraph_default_fb:admins"] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook admins'),
    '#default_value' => variable_get("auto_opengraph_default_fb:admins", ''),
    '#description' => t('A comma-separated list of numeric facebook ids'),
  );
  $form['facebook']["auto_opengraph_default_fb:app_id"] = array(
    '#type' => 'textfield',
    '#title' => t('Facebook application id'),
    '#default_value' => variable_get("auto_opengraph_default_fb:app_id", ''),
    '#description' => t('The numeric id for a facebook application'),
  );
  return $form;
}

function _auto_opengraph_settings_form_node($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Nodes') . (variable_get('auto_opengraph_node', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['auto_opengraph_node'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable auto opengraph for node pages'),
    '#default_value' => variable_get('auto_opengraph_node', TRUE),
  );

  _auto_opengraph_settings_form_get_fields('node', $og);

  // Get all the cck fields, using the widget provider's hook_auto_opengraph_widgets.
  // Every field specified here must be a CCK field with a widget provided by a module.
  $fields = content_fields();
  foreach ($fields as $field => $f) {
    if (function_exists($function = "{$f['widget']['module']}_auto_opengraph_widgets")) {
      $properties = $function($f['widget']['type']);
      if (!empty($properties)) {
        foreach ($properties as $property) {
          if (in_array($property, array_keys($og))) {
            $og[$property][$field] = $field;
          }
        }
      }
    }
  }

  $form = array_merge($form, _auto_opengraph_settings_form_render_fields('node', $og));

  $form['nodetypes'] = array(
    '#type' => 'fieldset',
    '#title' => t('Node types')
  );

  $nodetypes = node_get_types();
  foreach ($nodetypes as $nodetype => $data) {
    $title = $data->name;
    $form['nodetypes'][$nodetype] = array(
      '#type' => 'fieldset',
      '#title' => $title . (variable_get("auto_opengraph_node_item_$nodetype", TRUE) ? ' (enabled)' : ''),
      '#collapsed' => TRUE,
      '#collapsible' => TRUE,
    );
    $form['nodetypes'][$nodetype] = array_merge($form['nodetypes'][$nodetype], _auto_opengraph_settings_form_get_section_settings("node_item_$nodetype"));
    if (module_exists('og') && og_is_group_type($nodetype)) {
      $form['nodetypes'][$nodetype]['og'] = array(
        '#type' => 'fieldset',
        '#title' => t('Organic Groups'),
      );
      if (module_exists('fb_connect')) {
        $form['nodetypes'][$nodetype]['og']["auto_opengraph_node_item_{$nodetype}_use_og_admins"] = array(
          '#type' => 'select',
          '#options' => array(
            0 => t('none'),
            'default' => t('site default facebook admins only'),
            'default,owner' => t('site default admins and group owner'),
            'default,admins' => t('site default admins and all group administrators'),
            'owner' => t('group owner only'),
            'admins' => t('all group administrators'),
          ),
          '#title' => t('Administrators for Facebook page'),
          '#default_value' => variable_get("auto_opengraph_node_item_{$nodetype}_use_og_admins", 0),
        );
      }
      $form['nodetypes'][$nodetype]['og']["auto_opengraph_node_item_{$nodetype}_use_og_description"] = array(
        '#type' => 'checkbox',
        '#title' => t('Override OpenGraph description with Organic Groups description'),
        '#default_value' => variable_get("auto_opengraph_node_item_{$nodetype}_use_og_description", TRUE),
      );
    }
  }
  return $form;
}

/* TODO:
function _auto_opengraph_settings_form_user($og) {

} /* */

function _auto_opengraph_settings_form_taxonomy($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy') . (variable_get('auto_opengraph_taxonomy', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form = array_merge($form, _auto_opengraph_settings_form_get_section_settings('taxonomy'));
  if ($fields = _auto_opengraph_settings_form_get_fields('taxonomy', $og)) { // intentional assingment of $fields
    $form['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Property overrides'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['fields'] = array_merge($form['fields'], _auto_opengraph_settings_form_render_fields('taxonomy', $og));
  }
  return $form;
}

function _auto_opengraph_settings_form_other($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Other pages') . (variable_get('auto_opengraph_other', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form = array_merge($form, _auto_opengraph_settings_form_get_section_settings('other'));
  if ($fields = _auto_opengraph_settings_form_get_fields('other', $og)) { // intentional assingment of $fields
    $form['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Property overrides'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['fields'] = array_merge($form['fields'], _auto_opengraph_settings_form_render_fields('other', $og));
  }
  return $form;
}

function _auto_opengraph_settings_form_get_section_settings($section) {
  $form["auto_opengraph_$section"] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#default_value' => variable_get("auto_opengraph_$section", TRUE),
  );
  $form["auto_opengraph_{$section}_og_type"] = array(
    '#type' => 'textfield',
    '#size' => 15,
    '#title' => "Default opengraph type",
    '#default_value' => variable_get("auto_opengraph_{$section}_og_type", 'article'),
  );
  $form["auto_opengraph_{$section}_add_location"] = array(
    '#type' => 'select',
    '#options' => array(
      0 => t('None'),
      1 => t('Defaults only'),
      'each' => t('Override each'),
      'all' => t('Override all'),
      'fields' => t('Fields only'),
    ),
    '#title' => t('Add location headers'),
    '#default_value' => variable_get("auto_opengraph_{$section}_add_location", FALSE),
  );
  $form["auto_opengraph_{$section}_add_contact"] = array(
    '#type' => 'select',
    '#options' => array(
      0 => t('None'),
      1 => t('Defaults only'),
      'each' => t('Override each'),
      'all' => t('Override all'),
      'fields' => t('Fields only'),
    ),
    '#title' => t('Add contact headers'),
    '#default_value' => variable_get("auto_opengraph_{$section}_add_contact", FALSE),
  );
  $form["auto_opengraph_{$section}_add_fb"] = array(
    '#type' => 'select',
    '#options' => array(
      0 => t('None'),
      1 => t('Defaults only'),
      'each' => t('Override each'),
      'all' => t('Override all'),
      'fields' => t('Fields only'),
    ),
    '#title' => t('Add facebook headers (admins & app_id)'),
    '#default_value' => variable_get("auto_opengraph_{$section}_add_fb", FALSE),
  );
  return $form;
}

function _auto_opengraph_settings_form_get_fields($object, &$og) {

  // Get all module-defined fields, using the module's hook_auto_opengraph_fields.
  // The only requirement for a field is that $node->$field must exist at page render time.
  // see auto_opengraph.fields.inc for an example
  $fields = module_invoke_all('auto_opengraph_fields', $object);
  foreach ($fields as $field => $data) {
    foreach ($data['properties'] as $property) {
      if (in_array($property, array_keys($og))) {
        $og[$property][$field] = $field['title'];
      }
    }
  }

}

function _auto_opengraph_settings_form_render_fields($object, &$og) {
  $og = array_filter($og);
  $all_properties = auto_opengraph_properties();
  foreach ($og as $prop => $options) {
    if (stripos($prop, 'audio:') === 0 && !empty($og['audio'])) {
      $form['audio_meta']["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $form['audio_meta']['#type'] = 'fieldset';
      $form['audio_meta']['#title'] = t('Audio Meta');
      $form['audio_meta']['#collapsible'] = TRUE;
      $form['audio_meta']['#collapsed'] = TRUE;
    }
    elseif (stripos($prop, 'video:') === 0 && !empty($og['video'])) {
      $form['video_meta']["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $form['video_meta']['#type'] = 'fieldset';
      $form['video_meta']['#title'] = t('Video Meta');
      $form['video_meta']['#collapsible'] = TRUE;
      $form['video_meta']['#collapsed'] = TRUE;
    }
    elseif ($section = $all_properties[$prop]) { // intentional assignment of $section
      $form[$section]["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $title = $section == 'fb' ? 'Facebook' : ucfirst($section);
      $form[$section]['#type'] = 'fieldset';
      $form[$section]['#title'] = t($title);
      $form[$section]['#collapsible'] = TRUE;
      $form[$section]['#collapsed'] = TRUE;
    }
    else {
      $form["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
    }
  }
  $no_og = array_diff_key(auto_opengraph_properties(),$og);
  foreach ($no_og as $prop) {
    $form["auto_opengraph_{$object}_field_$prop"] = array(
      '#type' => 'value',
      '#value' => 0
    );
  }
  return $form;
}