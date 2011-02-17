<?php
// $Id$

/**
 * @file
 *
 */


function text_auto_opengraph_widgets($widget = NULL) {
  return array('type', 'description', 'audio:title', 'audio:artist', 'audio:album', 'latitude', 'longitude', 'street-address', 'locality', 'region', 'postal-code', 'country-name', 'email', 'phone_number', 'fax_number');
}

function number_auto_opengraph_widgets($widget = NULL) {
  return array('video:height', 'video:width');
}

function optionwidgets_auto_opengraph_widgets($widget = NULL) {
  return array('type');
}

function filefield_auto_opengraph_widgets($widget = NULL) {
  return array('video', 'audio');
}

function imagefield_auto_opengraph_widgets($widget = NULL) {
  return array('image');
}

function emvideo_auto_opengraph_widgets($widget = NULL) {
  return array('video');
}

function emaudio_auto_opengraph_widgets($widget = NULL) {
  return array('audio');
}

function emimage_auto_opengraph_widgets($widget = NULL) {
  return array('image');
}

function node_auto_opengraph_fields($object) {
  if ($object == 'node') {
    return array(
      'teaser' => array(
        'title' => 'Node: teaser',
        'properties' => array('description')
      ),
      'body' => array(
        'title' => 'Node: body',
        'properties' => array('description')
      ),
    );
  }
}

function location_auto_opengraph_fields($object) {
  if (module_exists('location')) {
    if ($object == 'node') {
      return array(
        'locations' => array(
          'title' => 'Node: location',
          'properties' => array('latitude', 'longitude', 'street-address', 'locality', 'region', 'postal-code', 'country-name')
        )
      );
    }
  }
}

function taxonomy_auto_opengraph_fields($object) {
  if (module_exists('taxonomy')) {
    switch($object) {
      case 'node':
        foreach (taxonomy_get_vocabularies() as $voc) {
          $ret["taxonomy|{$voc->vid}|single"] = array(
            'title' => "Taxonomy: $voc->name (one term only)",
            'properties' => array('type')
          );
          $ret["taxonomy|{$voc->vid}|multiple"] = array(
            'title' => "Taxonomy: $voc->name (multiple terms)",
            'properties' => array('description')
          );
        }
        return $ret;
        break;
    }
  }
}

function fb_auto_opengraph_fields($object) {
  if (module_exists('fb') && $object == 'node') {
    $ret['fb_author'] = array(
        'title' => t("Author's Facebook"),
        'properties' => array('fb:admins'),
    );
    if (module_exists('og')) {
      $ret['fb_og_admins'] = array(
        'title' => t("Author's Facebook and Organic Group admins (if any)"),
        'properties' => array('fb:admins'),
      );
    }
  }
}

function auto_opengraph_auto_opengraph_fields($object) {
  if ($object == 'node') {
    return array(
      'auto_opengraph_type' => array(
        'title' => t('Opengraph type selection'),
        'properties' => array('type'),
      ),
    );
  }
}