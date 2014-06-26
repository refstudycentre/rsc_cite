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
      break;
    case 'paragraphs':
      $node = rsc_cite_get_random('rsc_cite_paragraph');
      break;
  }
  
  if (!empty($node)) {
    
    $citation = field_view_field('node',$node,'rscc_body',array('label'=>'hidden'));
    
    $author = NULL;
    foreach($node as $fieldname => $items) {
      if (substr($fieldname,0,12) === 'rscc_author_') {
        $author = field_view_field('node',$node,$fieldname,array('label'=>'hidden'));
        $author['#weight'] = 10;
        break;
      }
    }
    
    if (empty($node->title)) {
      // no title, make body clickable
      $subject = "";
      $citation[0]['#markup'] = l($citation[0]['#markup'],"node/{$node->nid}");
    } else {
      // make title clickable
      $subject = l($node->title,"node/{$node->nid}");
    }
    
    $block = array(
      'subject' => $subject,
      'content' => array(
        'citation' => $citation,
        'author'   => $author,
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

