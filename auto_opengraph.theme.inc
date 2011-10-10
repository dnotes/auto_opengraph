<?php
// $Id$

/**
 * @file
 *
 */

function theme_auto_opengraph_image($variables) {
  $image = $variables['image'];
  if ($preset = variable_get('auto_opengraph_imagecache_preset', FALSE)) { // intentional assignment of $preset
    return check_url(url("imagecache/$preset/$image", array('absolute' => TRUE)));
  }
  else {
    return check_url(url("$image", array('absolute' => TRUE)));
  }
}
