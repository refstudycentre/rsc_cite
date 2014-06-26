<?php


/**
 * (re)set content type settings
 */
function rsc_cite_content_type_settings() {
  
  $content_types = array(
    'rsc_cite_short',
    'rsc_cite_paragraph',
  );
  
  foreach ($content_types as $type) {
    // settings from the content type manage page
    variable_set("node_preview_{$type}", 1);                  // Optional preview
    variable_set("node_options_{$type}", array());           // default unpublished
    variable_set("node_submitted_{$type}", 0);                // Unset 'Display author and date information'.
    variable_set("comment_{$type}", COMMENT_NODE_CLOSED);     // closed comments
    variable_set("menu_options_{$type}", array());           // no available menus
  }
  
}


/**
 * (re)create fields
 */
function rsc_cite_field_config($form_values = NULL) {
  
  $fieldnames = array_keys(field_info_fields());
  $prefix = 'rscc'; // RSC Cite

  // body text
  $fieldname = "{$prefix}_body";
  if (!in_array($fieldname,$fieldnames)) {
    field_create_field(array(
      'field_name'  => $fieldname,
      'type'        => 'text_long',
      'cardinality' => 1,
    ));
  }
  
  if (!empty($form_values['rsc_cite'])) {
    $settings = $form_values['rsc_cite'];        // use new settings
  } else {
    $settings = variable_get("rsc_cite",NULL);  // use old settings if any
  }

  if (!empty($settings)) {

    // taxonomy reference fields
    
    $cardinality = array(
      'author'     => 1,
      'tag'        => FIELD_CARDINALITY_UNLIMITED,
    );
    
    foreach($settings['vocabs'] as $field => $vid) {             // for each taxonomy reference field
      if ($vid > 0) {                                             // if a vocabulary has been configured
        $vocab = taxonomy_vocabulary_load($vid);
        $fieldname = "{$prefix}_{$field}_{$vocab->machine_name}"; // determine what this field should be called
        if (strlen($fieldname) <= 32) {
          if (!in_array($fieldname, $fieldnames)) {               // create it if it doesn't exist yet
            field_create_field(array(
              'field_name'  => $fieldname,
              'type'        => 'taxonomy_term_reference',
              'cardinality' => $cardinality[$field],
              'settings'    => array(
                'allowed_values' => array(
                  array(
                    'vocabulary' => $vocab->machine_name,
                    'parent'     => '0',
                  ),
                ),
              ),
            ));
            $fieldnames[] = $fieldname; // add field to list of names to prevent duplicate creation attempts
          }
        } else {
          drupal_set_message(t('Cannot create field @fieldname (more than 32 characters).', array('@fieldname' => $fieldname)),'error');
        }
      }
    }
  } else {
    drupal_set_message(t('Could not create fields for rsc_cite, because vocabulary settings have not been configured yet.'), 'error');
  }
  
}


/**
 * (re)configure field instances
 */
function rsc_cite_field_instance_config($form_values = NULL) {
  $t = get_t(); // this function might run during install, or any other time
  $prefix = "rscc";
  $fields = array_keys(field_info_fields());
  $instances_short = array_keys(field_info_instances('node','rsc_cite_short'));
  $instances_par = array_keys(field_info_instances('node','rsc_cite_paragraph'));
  
  if (!empty($form_values['rsc_cite'])) {
    $settings = $form_values['rsc_cite'];        // use new settings
  } else {
    $settings = variable_get("rsc_cite",NULL);  // use old settings if any
  }
  
  if (!empty($settings)) {
    if (!empty($settings['vocabs']['author']) && $settings['vocabs']['author'] > 0) {
      $author_vocab = taxonomy_vocabulary_load($settings['vocabs']['author']);
    }
    if (!empty($settings['vocabs']['tag']) && $settings['vocabs']['tag'] > 0) {
      $tag_vocab = taxonomy_vocabulary_load($settings['vocabs']['tag']);
    }
  }
  
  // Author term reference
  if (!empty($author_vocab)) {                                    // if author vocabulary has been configured
    $fieldname = "{$prefix}_author_{$author_vocab->machine_name}"; // determine what this field is called
    if (in_array($fieldname, $fields)) {                           // if the field exists
      
      // rsc_cite_short:
      if (!in_array($fieldname, $instances_short)) { // if the instance does not exist yet
        field_create_instance(array(                 // create the instance
          'field_name'  => $fieldname,
          'entity_type' => 'node',
          'bundle'      => 'rsc_cite_short',
          'label'       => $t('Author'),
          'required'    => false,
          'display'     => array(
            'default' => array(
              'label'  => 'hidden',
              'weight' => 10,
            ),
          ),
          'widget'      => array('type' => 'options_select'),
        ));
        $instances_short[] = $fieldname;
      }
      
      // rsc_cite_paragraph:
      if (!in_array($fieldname, $instances_par)) { // if the instance does not exist yet
        field_create_instance(array(               // create the instance
          'field_name'  => $fieldname,
          'entity_type' => 'node',
          'bundle'      => 'rsc_cite_paragraph',
          'label'       => $t('Author'),
          'required'    => false,
          'display'     => array(
            'default' => array(
              'label'  => 'hidden',
              'weight' => 10,
            ),
          ),
          'widget'      => array('type' => 'options_select'),
        ));
        $instances_par[] = $fieldname;
      }
      
    } else {
      drupal_set_message(t('Cannot create instances of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$author_vocab->machine_name)),'error');
    }
  }
  
  // Tags term reference
  if (!empty($tag_vocab)) {                                    // if tag vocabulary has been configured
    $fieldname = "{$prefix}_tag_{$tag_vocab->machine_name}"; // determine what this field is called
    if (in_array($fieldname, $fields)) {           // if the field exists
  
      // rsc_cite_paragraph:
      if (!in_array($fieldname, $instances_par)) { // if the instance does not exist yet
        field_create_instance(array(               // create the instance
          'field_name'  => $fieldname,
          'entity_type' => 'node',
          'bundle'      => 'rsc_cite_paragraph',
          'label'       => $t('Tags'),
          'required'    => false,
          'display'     => array(
            'default' => array(
              'type' => 'hidden',
            ),
          ),
          'widget'      => array('type' => 'options_select'),
        ));
        $instances_par[] = $fieldname;
      }
  
    } else {
      drupal_set_message(t('Cannot create instances of non-existing field <strong>@fieldname</strong> on taxonomy bundle <strong>@bundle</strong>.',array('@fieldname'=>$fieldname,'@bundle'=>$author_vocab->machine_name)),'error');
    }
  }

  /* Body text */
  $fieldname = "{$prefix}_body";
  
  // rsc_cite_paragraph:
  if (!in_array($fieldname, $instances_par) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'entity_type' => 'node',
      'bundle'      => 'rsc_cite_paragraph',
      'label'       => $t('Paragraph'),
      'required'    => false,
      'settings'    => array('text_processing' => 1), // allow text processing
      'display'     => array(
        'default' => array(
          'label'  => 'hidden',
          'weight' => 1,
        ),
      ),
    ));
    $instances_par[] = $fieldname;
  }
  
  // rsc_cite_short:
  if (!in_array($fieldname, $instances_short) && in_array($fieldname, $fields)) {
    field_create_instance(array(
      'field_name'  => $fieldname,
      'entity_type' => 'node',
      'bundle'      => 'rsc_cite_short',
      'label'       => $t('Quote'),
      'required'    => false,
      'settings'    => array('text_processing' => 0), // plain text
      'display'     => array(
        'default' => array(
          'label'  => 'hidden',
          'weight' => 1,
        ),
      ),
    ));
    $instances_short[] = $fieldname;
  }
  
}


/**
 * Delete unused fields that start with rscc_
 * Unused means: does not have any instances
 */
function rsc_cite_field_cleanup() {
  $fields = array_keys(field_info_fields());
  $used_fields = array_keys(field_info_field_map());
  $unused_fields = array_diff($fields,$used_fields);

  foreach ($unused_fields as $field) {
    if (substr($field,0,5) === 'rscc_') {
      field_delete_field($field);
    }
  }
}

