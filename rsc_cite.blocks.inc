<?php


/**
 * Implements hook_block_info().
 */
function rsc_cite_block_info() {

  $blocks['quotes'] = array(
    'info'  => t('RSC Cite: short quotes'),
    'cache' => DRUPAL_NO_CACHE,  // we cache the query result inside block_view
  );

  $blocks['paragraphs'] = array(
    'info'  => t('RSC Cite: paragraph quotes'),
    'cache' => DRUPAL_NO_CACHE,  // we cache the query result inside block_view
  );

  return $blocks;
}


/**
 * Implements hook_block_view().
 */
function rsc_cite_block_view($delta = '') {
  $block = array();
  
  switch ($delta) {
    case 'quotes':
      $node = rsc_cite_get_random('rsc_cite_short');
      $tag = 'span'; // inline
      break;
    case 'paragraphs':
      $node = rsc_cite_get_random('rsc_cite_paragraph');
      $tag = 'p';    // block
      break;
  }
  
  if (!empty($node)) {
    
    $citation = rsc_cite_field_get_safe_value($node,'rscc_body');
    
    $author = NULL;
    foreach($node as $fieldname => $items) {
      if (substr($fieldname,0,12) === 'rscc_author_') {
        $author = field_get_items('node',$node,$fieldname);
        $author = array_shift($author);
        $author = taxonomy_term_load($author['tid']);
      }
    }
    
    $block = array(
      'subject' => $node->title,
      'content' => array(
        '#theme'    => 'rsc_cite_block',
        '#tag'      => $tag,
        '#node'     => $node,
        '#citation' => $citation,
        '#author'   => $author,
      ),
    );
  }
  
  return $block;
}


/**
 * Get a random published node of the given content type
 */
function rsc_cite_get_random($bundle = NULL) {
  $node = NULL;
  if (!empty($bundle)) {
    
    $q = new EntityFieldQuery();
    $q->entityCondition('entity_type','node')  // node
      ->entityCondition('bundle',$bundle)      // content type
      ->propertyCondition('status',1)          // published
      ->addMetaData('account', user_load(1));  // run as user 1 (bypass access checks)
    $r = $q->execute();
    if (!empty($r['node'])) {
      $node = node_load(array_rand($r['node']));
    }
    
  }
  return $node;
}


/**
 * Get value of first field item of node
 */
function rsc_cite_field_get_safe_value($node, $fieldname) {
  $items = field_get_items('node',$node,$fieldname);
  if ($items) {
    $item = array_shift($items);
    if (!empty($item['safe_value'])) {
      return $item['safe_value'];
    }
  }
  return NULL;
}

