<?php
/**
 *
 * Alter hook_uc_checkout_pane()
 *
 */
function uc_address_items_uc_checkout_pane_alter( &$panes ) {

//    krumo($panes);

    $panes['billing']['callback']  = 'uc_address_items_checkout_pane_billing';
    $panes['delivery']['callback'] = 'uc_address_items_checkout_pane_delivery';

}


/**
 * Gets the delivery information.
 */
function uc_address_items_checkout_pane_delivery($op, $order, $form = NULL, &$form_state = NULL) {
  $description = t('Enter your delivery address and information here.');
  $copy = t('My delivery information is the same as my billing information.');
  return uc_address_items_checkout_pane_address('delivery', $op, $order, $form_state, $description, $copy);
}

/**
 * Gets the billing information.
 */
function uc_address_items_checkout_pane_billing($op, $order, $form = NULL, &$form_state = NULL) {
  $description = t('Enter your billing address and information here.');
  $copy = t('My billing information is the same as my delivery information.');
  return uc_address_items_checkout_pane_address('billing', $op, $order, $form_state, $description, $copy);
}

/**
 * Generic address pane handler.
 */
function uc_address_items_checkout_pane_address($pane, $op, $order, &$form_state, $description, $copy) {
  switch ($op) {
    case 'review':
//        krumo($order);
      $review[] = array('title' => t('Address'), 'data' => uc_address_items_order_address($order, $pane, FALSE));
      if (uc_address_field_enabled('phone') && !empty($order->{$pane . '_phone'})) {
        $review[] = array('title' => t('Phone'), 'data' => check_plain($order->{$pane . '_phone'}));
      }
      return $review;
    default:
        return uc_checkout_pane_address($pane, $op, $order, $form_state, $description, $copy);
  }
}