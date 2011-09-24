<?php
// $Id$

/**
 * @file
 *
 */


function auto_opengraph_image_effect_info() {
  $effects['aspect_ratio_crop'] = array(
    'label' => t('Aspect Crop'),
    'help' => t('Crop an image to an aspect ratio.'),
    'effect callback' => 'auto_opengraph_crop_effect',
    'form callback' => 'auto_opengraph_crop_form',
    'summary theme' => 'auto_opengraph_crop_summary',
  );
  return $effects;
}

function theme_auto_opengraph_crop($element) {
  $data = $element['#value'];
  return t('@aspect @fit@reversible', array('@fit' => t($data['fit']), '@aspect' => $data['aspect'], '@reversible' => ($data['reversible'] ? t(', or the reverse') : '')));
}

function auto_opengraph_crop_form($data = array()) {
  $form['aspect'] = array(
    '#type' => 'textfield',
    '#title' => t('Aspect ratio'),
    '#default_value' => isset($data['aspect']) ? $data['aspect'] : NULL,
    '#description' => t('An aspect ratio in the format width:height (e.g. 4:3)'),
    '#size' => 10,
  );
  $form['fit'] = array(
    '#type' => 'select',
    '#title' => t('Fit'),
    '#options' => array('max' => t('Maximum'), 'min' => t('Minimum'), 'exact' => t('Exact')),
    '#default_value' => isset($data['fit']) ? $data['fit'] : 'exact',
    '#description' => t('"Maximum" and "minimum" relate to the highest number of the ratio; for example,
                a maximum ratio of 3:4 will result in an image of 300x500px being cropped down to 300x400,
                even though 3:4 is technically greater than 3:5.
                An image with a minimum ratio of 1:1 will never be affected.')
  );
  $form['reversible'] = array(
    '#type' => 'checkbox',
    '#title' => t('or the reverse'),
    '#default_value' => isset($data['reversible']) ? $data['reversible'] : NULL,
    '#description' => t('i.e. for a value of 3:1, wide pictures would use 3:1 and tall pictures would use 1:3.')
  );
  $form['note'] = array( // opengraph images on facebook are required to be 50x50 or larger.
    '#value' => '<p>' . t('Note: since this preset was designed for use with opengraph media, it automatically enlarges images with height or width less than 50px.') . '</p>'
  );
  return $form;
}

function auto_opengraph_crop_effect(&$image, $data) {

  // opengraph images on facebook are required to be 50x50 or larger.
  if ($image->info['height'] < 50 || $image->info['width'] < 50) {
    $known = $image->info['height'] < $image->info['width'] ? 'height' : 'width';
    $unknown = $known == 'height' ? 'width' : 'height';
    $new[$known] = 50;
    $new[$unknown] = (50 * $image->info[$unknown]) / $image->info[$known];
    if (!imageapi_image_scale($image, $new['width'], $new['height'], TRUE)) {
      watchdog('imagecache', 'auto_opengraph_imagecache_crop failed to scale up to 50px. image: %image, data: %data.', array('%image' => $image->source, '%data' => print_r($data, TRUE)), WATCHDOG_ERROR);
      return FALSE;
    }
  }
  // end opengraph-specific code

  list($width, $height) = explode(':', $data['aspect']);
  $desired = $height / $width;
  $current = $image->info['height'] / $image->info['width'];
  if ($data['reversible'] && (($desired < 1 && $current > 1) || ($desired > 1 && $current < 1))) {
    $temp = $height;
    $height = $width;
    $width = $temp;
    $desired = $height / $width;
  }

  if ($current == $desired) return TRUE; // if the image is EXACTLY right, don't do anything.

  if ($desired < 1) {
    $wide = TRUE;
  }
  elseif ($desired > 1) {
    $tall = TRUE;
  }
  else {
    $square = TRUE;
  }

  $min = $data['fit'] == 'min';
  $max = $data['fit'] == 'max';

  if ($max && $tall && ($current < $desired)) return TRUE;
  if ($min && $tall && ($current > $desired)) return TRUE;
  if ($max && $wide && ($current > $desired)) return TRUE;
  if ($min && $wide && ($current < $desired)) return TRUE;
  if ($square && $min) return TRUE; // images with a minimum ratio of 1:1 will never be processed.

  $min = ($min || $data['fit'] == 'exact');
  $max = ($max || $data['fit'] == 'exact');

  // Trim the side
  if (($wide && $max) || ($tall && $min) || ($square && $max)) {
    $h = $image->info['height'];
    $w = round(($h / $height) * $width);
  }
  
  // Trim the bottom
  else {
    $w = $image->info['width'];
    $h = round(($w / $width) * $height);
  }
  
  if(!imageapi_image_crop($image, 'center', 'center', $w, $h)) {
    watchdog('imagecache', 'auto_opengraph_imagecache_crop failed. image: %image, data: %data.', array('%image' => $image->source, '%data' => print_r($data, TRUE)), WATCHDOG_ERROR);
    return FALSE;
  }
  return TRUE;
}
