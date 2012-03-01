<?php
/**
 *
 * Implementation of hook_uc_order
 * @arg $op
 * @arg $arg1 order instance
 * @arg $arg2 $edit
 *
 * @see uc_order.module, function uc_order_module_invoke
 *
 */
function uc_address_items_uc_order($op, &$arg1, $arg2) {

  $types = uc_address_items_get_types();
//debug($op);
  switch ($op) {
    case "load":

        $address_addons = array_keys(uc_address_items_list_labels());
        foreach ($types as $prefix) {
            $values = db_select('uc_address_items', 'a')
                ->fields('a')
                ->condition('order_id', $arg1->order_id)
                ->condition('type', $prefix)
                ->execute()
                ->fetchAssoc();
            foreach ($address_addons as $key) {
                $val = '';
                if(isset( $values[$key] )) {
                    $val = $values[$key];
                }
                $arg1->{$prefix.'_'.$key} = $val;
            }
        }
        
        break;
    case "new":

        $address_addons = array_keys(uc_address_items_list_labels());

        foreach ($address_addons as $key) {
            foreach ($types as $prefix) {
                if(!isset( $arg1->{$prefix.'_'.$key} )) {
                   $arg1->{$prefix.'_'.$key} = '';
                }
            }
        }

        break;
    case "save":
    case "update":
//debug($op);
        $address_addons = array_keys(uc_address_items_list_labels());       
        foreach ($types as $prefix) {
            $values = array(
                "order_id"  =>  $arg1->order_id,
                "type"      =>  $prefix,
            );
            foreach ($address_addons as $key) {
                if(isset( $arg1->{$prefix.'_'.$key} )) {
                   $values[ $key ] = $arg1->{$prefix.'_'.$key};
                }
            }

            $id = db_select('uc_address_items', 'a')
                    ->fields('a', array( 'id' ))
                    ->condition('type', $prefix)
                    ->condition('order_id', $arg1->order_id)
                    ->execute()
                    ->fetchAssoc();

            if(!$id){
                db_insert('uc_address_items')
                    ->fields( $values )
                    ->execute();
            }
            else{
                db_update('uc_address_items')
                    ->fields( $values )
                    ->condition('id', $id)
                    ->execute();
            }
            
        }

        break;
    case "delete":

        foreach ($types as $prefix) {
            db_delete('uc_address_items')
                ->condition('order_id', $arg1->order_id)
                ->condition('type', $prefix)
                ->execute();
        }

        break;
    case "can_update":
    case "can_delete":
    case "total":
    case "submit":
//      debug($op);
//      debug($arg1);
//      debug($arg2);
      break;
  }
}

/**
 *
 * Alter hook_uc_order_pane()
 *
 */
function uc_address_items_uc_order_pane_alter(&$panes) {


  $panes['ship_to']['callback'] = 'uc_address_items_pane_ship_to';
  $panes['bill_to']['callback'] = 'uc_address_items_pane_bill_to';

}



/**
 * Handles the "Ship to" order pane.
 */
function uc_address_items_pane_ship_to($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'customer':
      if (!uc_order_is_shippable($order)) {
        return;
      }
    case 'view':
      $build = array('#markup' => uc_address_items_order_address($order, 'delivery') . '<br />' . check_plain($order->delivery_phone));
      return $build;
    default:
        return uc_order_pane_ship_to($op, $order, $form, $form_state);
  }
}

/**
 * Handles the "Bill to" order pane.
 */
function uc_address_items_pane_bill_to($op, $order, &$form = NULL, &$form_state = NULL) {
  switch ($op) {
    case 'view':
    case 'customer':
      $build = array('#markup' => uc_address_items_order_address($order, 'billing') . '<br />' . check_plain($order->billing_phone));
      return $build;

    default:
        return uc_order_pane_bill_to($op, $order, $form, $form_state);
  }
}
