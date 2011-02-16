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

  foreach (array('settings', 'defaults', 'facebook', 'node', 'user', 'taxonomy', 'other') as $part) {
    if (function_exists($function = "_auto_opengraph_settings_form_$part")) {
      $form[$part] = call_user_func($function, $og);
    }
  }

  return system_settings_form($form);
}

function _auto_opengraph_settings_form_settings() {

  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Settings'),
    '#collapsible' => TRUE,
  );

  $form['auto_opengraph_front_page'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('auto_opengraph_front_page', TRUE),
    '#title' => t('Implement site-wide tags for front page')
  );
  $form['auto_opengraph_site_type'] = array(
    '#type' => 'select',
    '#default_value' => variable_get('auto_opengraph_site_type', 'website'),
    '#title' => t('Site type'),
    '#options' => array('website' => t('website'), 'blog' => t('blog')),
  );
  $form['auto_opengraph_disable_for_tabs'] = array(
    '#type' => 'checkbox',
    '#default_value' => variable_get('auto_opengraph_disable_for_tabs', TRUE),
    '#title' => t('Disable opengraph meta tags for tabs (i.e. node/1/edit)'),
    '#description' => t('Auto opengraph does not create opengraph tags on tab pages such as node or user edit pages.  If you want to implement your own system, you will need to siable this option.')
  );

//  $form['auto_opengraph_advanced'] = array(
//    '#type' => 'checkbox',
//    '#default_value' => variable_get('auto_opengraph_advanced', TRUE),
//    '#title' => t('Use advanced mode for node types'),
//    '#description' => t('For most use cases, the default functionality should be sufficient.<br/>  Advanced mode allows you to set fields for each individual node type.')
//  );

  return $form;
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

  $form = array_merge($form, _auto_opengraph_settings_form_property_fields('node', $og));

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
    $form['nodetypes'][$nodetype] = array_merge($form['nodetypes'][$nodetype], _auto_opengraph_settings_form_get_section_settings("node_item_$nodetype", $og));
  }
  return $form;
}

/* TODO:
function _auto_opengraph_settings_form_user($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Users') . (variable_get('auto_opengraph_user', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form = array_merge($form, _auto_opengraph_settings_form_get_section_settings('user', $og));
  $form = array_merge($form, _auto_opengraph_settings_form_property_fields('user', $og));
  return $form;
} /* */

function _auto_opengraph_settings_form_taxonomy($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Taxonomy') . (variable_get('auto_opengraph_taxonomy', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form = array_merge($form, _auto_opengraph_settings_form_get_section_settings('taxonomy', $og));
  $form = array_merge($form, _auto_opengraph_settings_form_property_fields('taxonomy', $og));
  return $form;
}

function _auto_opengraph_settings_form_other($og) {
  $form = array(
    '#type' => 'fieldset',
    '#title' => t('Other pages') . (variable_get('auto_opengraph_other', TRUE) ? t(' (enabled)') : ''),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form = array_merge($form, _auto_opengraph_settings_form_get_section_settings('other', $og));
  $form = array_merge($form, _auto_opengraph_settings_form_property_fields('other', $og));
  return $form;
}

function _auto_opengraph_settings_form_get_section_settings($section, $og) {
  $form["auto_opengraph_$section"] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#default_value' => variable_get("auto_opengraph_$section", TRUE),
  );
  $form["auto_opengraph_{$section}_og_type"] = array(
    '#type' => 'textfield',
    '#size' => 10,
    '#title' => "Default opengraph type",
    '#default_value' => variable_get("auto_opengraph_{$section}_og_type", 'article'),
  );
  if (!empty($og['type'])) {
    $form["auto_opengraph_{$section}_og_type_field"] = array(
      '#type' => 'select',
      '#title' => "Field to override default type",
      '#default_value' => variable_get("auto_opengraph_{$section}_og_type_field", FALSE),
      '#options' => array_merge(array(0 => t('none')), $og['type']),
    );
  }
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
  $each = stripos($section, 'ode') ? 'node' : $section;
  $form["auto_opengraph_{$section}_add_fb"] = array(
    '#type' => 'select',
    '#options' => array(
      0 => t('None'),
      1 => t('Defaults only'),
      'each' => t('Override each'),
      'all' => t('Override all'),
      'fields' => t('Fields only'),
      'choose' => t("Choose for each $each"),
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
        $og[$property][$field] = $data['title'];
      }
    }
  }

}

function _auto_opengraph_settings_form_property_fields($object, $og) {
  _auto_opengraph_settings_form_get_fields('other', $og);
  $og = array_filter($og);
  $all_properties = auto_opengraph_properties();
  foreach ($og as $prop => $options) {
    if (stripos($prop, 'audio:') === 0 && !empty($og['audio'])) {
      $fields['audio_meta']["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $fields['audio_meta']['#type'] = 'fieldset';
      $fields['audio_meta']['#title'] = t('Audio Meta');
      $fields['audio_meta']['#collapsible'] = TRUE;
      $fields['audio_meta']['#collapsed'] = TRUE;
    }
    elseif (stripos($prop, 'video:') === 0 && !empty($og['video'])) {
      $fields['video_meta']["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $fields['video_meta']['#type'] = 'fieldset';
      $fields['video_meta']['#title'] = t('Video Meta');
      $fields['video_meta']['#collapsible'] = TRUE;
      $fields['video_meta']['#collapsed'] = TRUE;
    }
    elseif ($section = $all_properties[$prop]) { // intentional assignment of $section
      $fields[$section]["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
      $title = $section == 'fb' ? 'Facebook' : ucfirst($section);
      $fields[$section]['#type'] = 'fieldset';
      $fields[$section]['#title'] = t($title);
      $fields[$section]['#collapsible'] = TRUE;
      $fields[$section]['#collapsed'] = TRUE;
    }
    else {
      $fields["auto_opengraph_{$object}_field_$prop"] = array(
        '#type' => 'select',
        '#options' => array_merge(array(0 => 'none'), $options),
        '#default_value' => variable_get("auto_opengraph_{$object}_field_$prop", 0),
        '#title' => t(str_replace(':', ' ', "set opengraph <b>$prop</b> from"))
      );
    }
  }
  $no_og = array_diff_key(auto_opengraph_properties(),$og);
  foreach ($no_og as $prop) {
    $fields["auto_opengraph_{$object}_field_$prop"] = array(
      '#type' => 'value',
      '#value' => 0
    );
  }
  foreach ($fields as $field) {
    if ($field['#type'] && $field['#type'] != 'value') {
      $expand = TRUE;
    }
  }
  if ($expand) { // intentional assingment of $fields
    $form['fields'] = array(
      '#type' => 'fieldset',
      '#title' => t('Property overrides'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['fields'] = array_merge($form['fields'], $fields);
  }
  else {
    $form = $fields;
  }
  return $form;
}