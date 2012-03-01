<?php

/**
 * @file
 * Token hooks for the uc_address_items module.
 */

/**
 * Implements hook_token_info().
 */
function uc_address_items_token_info() {
  $type = array(
    'name' => t('Address items information'),
    'description' => t('Tokens for address items.'),
  );

  $address_items = array(); 
  $address = uc_address_items_list_labels();
  foreach ($address as $key => $val) {
    $address_items[$key] = array(
    'name' => t($val),
    'description' => t('Address item: !val.', array('!val'=>$val)),
    );
  }
  
  return array(
    'types' => array(
        'store_address_items' => $type
    ),
    'tokens' => array(
      'store_address_items' => $address_items,
    ),
  );
}

/**
 * Implements hook_tokens().
 */
function uc_address_items_tokens($type, $tokens, $data = array(), $options = array()) {
  global $base_url;

  $replacements = array();

  if ($type == 'store_address_items') {

      $address_items = array();
      $address = uc_address_items_get_list();
      
      foreach ($address as $key => $val) {
        $replacements[$key] = variable_get("uc_store_".$key, '');
      }
  }

  return $replacements;
}

function uc_address_items_tokens_alter(array &$replacements, array $context) {

    switch($context['type']){
        case "store":

            if(isset($replacements['address'])) {
                $replacements['address'] = uc_address_items_address();
            }


            break;
        case "uc_order":

            $types = uc_address_items_get_types();
            foreach ($types as $type) {
                $replacements[$type.'-address'] = uc_address_items_order_address( $context['data']['uc_order'], $type );
            }
//            krumo($replacements[$type.'-address'], $context['data']['uc_order'], $type);

            break;
        
    }




//    krumo($replacements, $context, '---------------------------------------------------------------');

}