<?php

/**
 * Implements hook_uc_payment_method().
 */
function uc_address_items_uc_payment_method_alter( &$methods ) {

  if(isset($methods['check'])) {
    $methods['check']['callback'] = 'uc_address_items_payment_method_check';
  }

  return $methods;
}


/**
 * Payment method callback for the "Check" payment method.
 */
function uc_address_items_payment_method_check($op, &$order, $form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'cart-details':
      $build['instructions'] = array(
        '#markup' => t('Checks should be made out to:')
      );

      if (!variable_get('uc_check_mailing_street1', FALSE)) {
        $build['address'] = array(
          '#markup' => uc_address_items_format(
            uc_store_name(),
            NULL,
            variable_get('uc_store_company', ''),
            variable_get('uc_store_street1', ''),
            variable_get('uc_store_street2', ''),
            variable_get('uc_store_city', ''),
            variable_get('uc_store_zone', ''),
            variable_get('uc_store_postal_code', ''),
            variable_get('uc_store_country', 840),
            uc_address_items_get_list('uc_store_')
          ),
          '#prefix' => '<p>',
          '#suffix' => '</p>',
        );
      }
      else {
        $build['address'] = array(
          '#markup' => uc_address_items_format(
            variable_get('uc_check_mailing_name', ''),
            NULL,
            variable_get('uc_check_mailing_company', ''),
            variable_get('uc_check_mailing_street1', ''),
            variable_get('uc_check_mailing_street2', ''),
            variable_get('uc_check_mailing_city', ''),
            variable_get('uc_check_mailing_zone', ''),
            variable_get('uc_check_mailing_postal_code', ''),
            variable_get('uc_check_mailing_country', 840),
            uc_address_items_get_list('uc_check_mailing_')
          ),
          '#prefix' => '<p>',
          '#suffix' => '</p>',
        );
      }

      $build['policy'] = array(
        '#markup' => '<p>' . variable_get('uc_check_policy', '') . '</p>'
      );
      return $build;

    case 'cart-review':
      if (!variable_get('uc_check_mailing_street1', FALSE)) {
        $review[] = array(
          'title' => t('Mail to'),
          'data' => uc_address_items_format(
            uc_store_name(),
            NULL,
            variable_get('uc_store_company', ''),
            variable_get('uc_store_street1', ''),
            variable_get('uc_store_street2', ''),
            variable_get('uc_store_city', ''),
            variable_get('uc_store_zone', ''),
            variable_get('uc_store_postal_code', ''),
            variable_get('uc_store_country', 840),
            uc_address_items_get_list('uc_store_')
          )
        );
      }
      else {
        $review[] = array(
          'title' => t('Mail to'),
          'data' => uc_address_items_format(
            variable_get('uc_check_mailing_name', ''),
            NULL,
            variable_get('uc_check_mailing_company', ''),
            variable_get('uc_check_mailing_street1', ''),
            variable_get('uc_check_mailing_street2', ''),
            variable_get('uc_check_mailing_city', ''),
            variable_get('uc_check_mailing_zone', ''),
            variable_get('uc_check_mailing_postal_code', ''),
            variable_get('uc_check_mailing_country', 840),
            uc_address_items_get_list('uc_check_mailing_')
          )
        );
      }
      return $review;

    case 'order-view':
      $build = array('#suffix' => '<br />');

      $result = db_query('SELECT clear_date FROM {uc_payment_check} WHERE order_id = :id ', array(':id' => $order->order_id));
      if ($clear_date = $result->fetchField()) {
        $build['#markup'] = t('Clear Date:') . ' ' . format_date($clear_date, 'uc_store');
      }
      else {
        $build['#markup'] = l(t('Receive Check'), 'admin/store/orders/' . $order->order_id . '/receive_check');
      }
      return $build;

    case 'customer-view':
      $build = array();
      $result = db_query('SELECT clear_date FROM {uc_payment_check} WHERE order_id = :id ', array(':id' => $order->order_id));
      if ($clear_date = $result->fetchField()) {
        $build['#markup'] = t('Check received') . '<br />' .
          t('Expected clear date:') . '<br />' . format_date($clear_date, 'uc_store');
      }
      return $build;

    case 'settings':
      $form['check_address_info'] = array(
        '#markup' => '<div>' . t('Set the mailing address to display to customers who choose this payment method during checkout.') . '</div>',
      );
      $form['uc_check_mailing_name'] = uc_textfield(t('Contact'), variable_get('uc_check_mailing_name', ''), FALSE, t('Direct checks to a person or department.'), 128);

      $form['uc_check_address'] = array(
        '#type' => 'uc_address',
        '#default_value' => array(
          'uc_check_mailing_company' => variable_get('uc_check_mailing_company', ''),
          'uc_check_mailing_street1' => variable_get('uc_check_mailing_street1', ''),
          'uc_check_mailing_street2' => variable_get('uc_check_mailing_street2', ''),
          'uc_check_mailing_city' => variable_get('uc_check_mailing_city', ''),
          'uc_check_mailing_zone' => variable_get('uc_check_mailing_zone', ''),
          'uc_check_mailing_country' => isset($form_state['values']['uc_check_mailing_country']) ? $form_state['values']['uc_check_mailing_country'] : variable_get('uc_check_mailing_country', ''),
          'uc_check_mailing_postal_code' => variable_get('uc_check_mailing_postal_code', ''),
        ),
        '#required' => FALSE,
        '#key_prefix' => 'uc_check_mailing',
      );

      // here load uc_address_items addons
      $prefix = $form['uc_check_address']['#key_prefix'].'_';
      $_uc_address_items_check = uc_address_items_get_list( $prefix );
      $uc_address_items_check = array();
      foreach ($_uc_address_items_check as $c_key => $c_value) {
        $uc_address_items_check[ $prefix.$c_key ] = $c_value;
      }
      $form['uc_check_address']['#default_value'] += $uc_address_items_check;

//      krumo($form['uc_check_address']['#default_value'], $uc_address_items_check);
      
      $form['uc_check_policy'] = array(
        '#type' => 'textarea',
        '#title' => t('Check payment policy', array(), array('context' => 'cheque')),
        '#description' => t('Instructions for customers on the checkout page.'),
        '#default_value' => variable_get('uc_check_policy', t('Personal and business checks will be held for up to 10 business days to ensure payment clears before an order is shipped.')),
        '#rows' => 3,
      );
      return $form;
  }
}