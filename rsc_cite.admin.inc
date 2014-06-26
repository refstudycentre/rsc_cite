<?php


/**
 * Form builder for the module settings page
 */
function rsc_cite_settings() {
  $form = array();

  /* collect info from site */
  $vocabs = taxonomy_get_vocabularies();
  $vocab_options = array('-1' => t('None'));
  foreach($vocabs as $vid => $vocab) {
    $vocab_options[$vid] = $vocab->name;
  }

  /* collect info from this module */
  $default_settings = array(
    'vocabs' => array(
      'tag'    => '-1',
      'author' => '-1',
    ),
  );
  $settings = variable_get("rsc_cite",$default_settings);

  $links = array();
  $links["admin/structure/taxonomy/add"] = t('Add a new vocabulary');
  $link_items = array();
  foreach($links as $path => $text) {
    $link_items[] = l($text,$path);
  }
  $link_list = array(
    '#title' => t('Related configuration pages:'),
    '#theme' => 'item_list',
    '#items' => $link_items,
  );

  /* build the form */
  $form['#tree'] = TRUE; // save the whole thing as an array

  $form['rsc_cite']['links'] = array(
    '#type'   => 'markup',
    '#markup' => drupal_render($link_list),
    '#prefix' => '<div class="form-item">',
    '#suffix' => '</div>',
  );

  $form['rsc_cite']['vocabs'] = array(
    '#type'  => 'fieldset',
    '#title' => 'Taxonomy vocabulary to use for...',
  );

  $form['rsc_cite']['vocabs']['author'] = array(
    '#type'          => 'select',
    '#required'      => FALSE,
    '#multiple'      => FALSE,
    '#title'         => t('authors'),
    '#description'   => t('A reference field to terms in this vocabulary will be created on citation content types.'),
    '#default_value' => $settings['vocabs']['author'],
    '#options'       => $vocab_options,
  );

  $form['rsc_cite']['vocabs']['tag'] = array(
    '#type'          => 'select',
    '#required'      => FALSE,
    '#multiple'      => FALSE,
    '#title'         => t('tags'),
    '#description'   => t('A reference field to terms in this vocabulary will be created on citation content types.'),
    '#default_value' => $settings['vocabs']['tag'],
    '#options'       => $vocab_options,
  );

  $form['rsc_cite']['actions'] = array(
    '#type'          => 'radios',
    '#title'         => 'Actions:',
    '#description'   => t('Choose an optional action to perform when submitting.'),
    '#default_value' => 'Nothing',
    '#options'       => array(
      0                     => 'Nothing',
      'content_type_reset'     => 'Reset the content type settings.',
      'fields_create'          => '(re)Create missing fields and instances',
      'fields_cleanup'         => 'Delete (clean up) unused fields',
    ),
    'content_type_reset' => array('#description' => t("Default settings include: unpublished by default, comments open, etc.")),
    'fields_create'      => array('#description' => t("Old fields and field instances will not be deleted automatically. If you are experiencing trouble with a field or field instance, delete it, and the choose this option. YOU WILL LOSE ALL VALUES CONTAINED IN EVERY INSTANCE OF A FIELD IF YOU DELETE IT.")),
    'fields_cleanup'     => array('#description' => t("This will delete all fields created by rsc_cite which do not have any instances (i.e. are not linked to a content type any more). YOU WILL LOSE ALL DATA IN THESE FIELDS.")),
  );

  $form['#submit'][] = 'rsc_cite_settings_submit';
  return system_settings_form($form);
}


/**
 * Submit handler for the module settings page
 */
function rsc_cite_settings_submit($form, &$form_state) {
  if (!empty($form_state['values']['rsc_cite'])) {
    $values = $form_state['values']['rsc_cite'];
    switch($values['actions']) {
      case 'content_type_reset':
        
        module_load_include('inc','rsc_cite','rsc_cite.common');
        rsc_cite_content_type_settings();
        drupal_set_message("Settings for content types rsc_cite_* have been reset.");
        
        break;
      case 'fields_cleanup':
        
        module_load_include('inc','rsc_cite','rsc_cite.common');
        rsc_cite_field_cleanup();
        drupal_set_message("Unused fields from rsc_cite have been deleted.");
        
        break;
      case 'fields_create':
        
        module_load_include('inc','rsc_cite','rsc_cite.common');
        rsc_cite_field_config($form_state['values']);
        rsc_cite_field_instance_config($form_state['values']);
        drupal_set_message("Fields and instances for rsc_cite have been created.");
        
        break;
    }
  }
}

