<?php
// $Id$

/**
 * @file
 *
 */

function _auto_opengraph_add_basic(&$og) {
  if (!$og['site_name']) { $og['site_name'] = check_plain(variable_get('site_name', 'Drupal')); }
  if (!$og['title']) { $og['title'] = drupal_get_title(); }
  if (!$og['type']) { $og['type'] = 'article'; }
  if (!$og['image'] && $image = variable_get('auto_opengraph_default_image', '')) { $og['image'] = $image; } // TODO: image upload for default
  if ($og['image']) { $og['image'] = theme('auto_opengraph_image', $og['image']); }
  if (!$og['url']) { $og['url'] = $vars['path']; }
  if (!$og['description'] && $description = variable_get('auto_opengraph_default_description', '')) { $og['description'] = $description; }
}

function _auto_opengraph_add_default(&$og, $property) {
  if (!$og[$property] && ($value = variable_get("auto_opengraph_default_$property", ''))) {
    $og[$property] = check_plain($value);
  }
}

function _auto_opengraph_add_location(&$og) {
  foreach(array('latitude', 'longitude', 'street-address', 'locality', 'region', 'postal-code', 'country-name') as $property) {
    _auto_opengraph_add_default($og, $property);
  }
}

function _auto_opengraph_add_contact(&$og) {
  foreach(array('email', 'phone_number', 'fax_number') as $property) {
    _auto_opengraph_add_default($og, $property);
  }
}

function _auto_opengraph_add_fb(&$og) {
  foreach(array('fb:admins', 'fb:app_id') as $property) {
    _auto_opengraph_add_default($og, $property);
  }
}

