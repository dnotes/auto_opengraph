<?php
// $Id$

/**
 * @file
 *
 */

function theme_auto_opengraph_image($image) {
  if ($preset = variable_get('auto_opengraph_imagecache_preset', FALSE)) { // intentional assignment of $preset
    return check_url(url(file_directory_path() . "/imagecache/$preset/$image", array('absolute' => TRUE)));
  }
  else {
    return check_url(url(file_directory_path() . "/$image", array('absolute' => TRUE)));
  }
}

function theme_auto_opengraph_widget_text_value($data, $property, $widget = NULL) {
  return $data[0]['value'];
}

function theme_auto_opengraph_widget_number_value($data, $property, $widget = NULL) {
  return $data[0]['value'];
}

function theme_auto_opengraph_widget_optionwidgets_value($data, $property, $widget = NULL) {
  return $data[0]['value'];
}

function theme_auto_opengraph_widget_filefield_value($data, $property, $widget = NULL) {
  return $data[0]['filepath'];
}

function theme_auto_opengraph_widget_imagefield_value($data, $property, $widget = NULL) {
  return $data[0]['filepath'];
}

function theme_auto_opengraph_widget_emvideo_value($data, $property, $widget = NULL) {
  switch ($property) {
    case 'video':
      if ($code = emfield_include_invoke("emvideo", $data[0]['provider'], 'video', $data[0]['value'], $data[0]['data'])) {
        $matches = array();
        $video = preg_match('|data=["\']([^&"\']+)|', $code, $matches);
        return $matches[1];
      }
      break;
    default:
      
      break;
  }
}

function theme_auto_opengraph_widget_emaudio_value($data, $property, $widget = NULL) {
  $value = emfield_include_invoke("emaudio", $data[0]['provider'], 'extract', $data[0]['value'], $data[0]['data']);
  return is_array($value) ? $value[0] : FALSE;
}

function theme_auto_opengraph_widget_emimage_value($data, $property, $widget = NULL) {
  $value = emfield_include_invoke("emimage", $data[0]['provider'], 'extract', $data[0]['value'], $data[0]['data']);
  return is_array($value) ? $value[0] : FALSE;
}

function theme_auto_opengraph_node_teaser_value($data, $property, $field) {
  return strip_tags($data->teaser);
}

function theme_auto_opengraph_node_body_value($data, $property, $field) {
  return strip_tags($data->body);
}

function theme_auto_opengraph_node_locations_value($data, $property, $field) {
  switch ($property) {
    case 'street-address':
      return "{$data->locations[0]['street']}, {$data->locations[0]['additional']}";
    case 'locality':
      return $data->locations[0]['city'];
    case 'postal-code':
      return $data->locations[0]['postal_code'];
    case 'country-name':
      return $data->locations[0]['country_name'];
    default:
      return $data->locations[0][$property];
  }
}

function theme_auto_opengraph_node_taxonomy_value($data, $property, $field) {
  if ($field[2] == 'single') {
    $y = $n;
  }
}