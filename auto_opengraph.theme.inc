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
  $text = strip_tags($data->teaser);
  $ret = array();
  $num = preg_match('|([^.!?][.!?][^.!?][.!?])|', $text, $ret);
  if (is_array($ret)) {
    return $ret;
  }
  elseif ($data->teaser) {
    return $data->teaser;
  }
  else {
    return variable_get('auto_opengraph_default_description', FALSE);
  }
}

function theme_auto_opengraph_node_body_value($data, $property, $field) {
  $text = strip_tags($data->body);
  $ret = array();
  $num = preg_match('|([^.!?][.!?][^.!?][.!?])|', $text, $ret);
  if (is_array($ret)) {
    return $ret;
  }
  elseif ($data->body) {
    return $data->body;
  }
  else {
    return variable_get('auto_opengraph_default_description', FALSE);
  }
}

function theme_auto_opengraph_node_locations_value($data, $property, $field) {
  switch ($property) {
    case 'street-address':
      $ret = $data->locations[0]['street'];
      if ($additional = $data->locations[0]['additional']) $ret .= ", $additional"; // intentional assignment of $additional
      return $ret;
    case 'locality':
      return $data->locations[0]['city'];
    case 'postal-code':
      return $data->locations[0]['postal_code'];
    case 'country-name':
      return $data->locations[0]['country_name'];
    case 'region':
      return $data->locations[0]['province_name'];
    default:
      return $data->locations[0][$property];
  }
}

function theme_auto_opengraph_node_taxonomy_value($data, $property, $field) {
  if ($field[2] == 'single') {
    $y = $n;
  }
}

function theme_auto_opengraph_node_fb_author_value($data, $property, $field) {
  if ($fbu = fb_get_fbu($data->uid)) {
    return $fbu;
  }
}

function theme_auto_opengraph_node_fb_og_admins_value($data, $property, $field) {
  if (module_exists('og') && module_exists('fb')) {
    $sql = og_list_users_sql(TRUE, TRUE, '');
    $q = db_query($sql, $data->nid);
    while ($u = db_fetch_object($q)) {
      if ($fbu = fb_get_fbu($u->uid)) { // intentional assignment of $fbu
        $ids[] = $fbu;
      }
    }
    if (!empty($ids)) {
      return implode(',', $ids);
    }
  }
}