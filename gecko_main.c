/***************************************************************************//**
 * @file
 * @brief Silicon Labs BT Mesh Empty Example Project
 * This example demonstrates the bare minimum needed for a Blue Gecko BT Mesh C application.
 * The application starts unprovisioned Beaconing after boot
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/* Board headers */
#include "init_mcu.h"
#include "init_board.h"
#include "init_app.h"
#include "ble-configuration.h"
#include "board_features.h"

/* Bluetooth stack headers */
#include "bg_types.h"
#include "native_gecko.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>
#include<stdbool.h>

/* Libraries containing default Gecko configuration values */
#include "em_emu.h"
#include "em_cmu.h"
#include <em_gpio.h>

/* Device initialization header */
#include "hal-config.h"

#if defined(HAL_CONFIG)
#include "bsphalconfig.h"
#else
#include "bspconfig.h"
#endif
#include "src/ble_mesh_device_type.h"


#include "src/gpio.h"
#include "src/log.h"
#include "src/display.h"
#include "src/letimer.h"
#include "src/i2c_config.h"


#include "mesh_generic_model_capi_types.h"

//#include "mesh_lib.h"

//#define FACTORY_RESET_ID 78
//#define TIMER_RESTART_ID 77
//
///***********************************************************************************************//**
// * @addtogroup Application
// * @{
// **************************************************************************************************/
//
///***********************************************************************************************//**
// * @addtogroup app
// * @{
// **************************************************************************************************/
//
//// bluetooth stack heap
//#define MAX_CONNECTIONS 2
//
//uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];
//
//// Bluetooth advertisement set configuration
////
//// At minimum the following is required:
//// * One advertisement set for Bluetooth LE stack (handle number 0)
//// * One advertisement set for Mesh data (handle number 1)
//// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
//// * One advertisement set for Mesh unprovisioned URI (handle number 3)
//// * N advertisement sets for Mesh GATT service advertisements
//// (one for each network key, handle numbers 4 .. N+3)
////
//#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)
//
//extern uint8_t pin_state;
//extern uint8_t fall_state;
//
//
//uint16_t index = 0x00;
//
//uint8_t transaction_id = 0;
//
//static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;
//
//// bluetooth stack configuration
//extern const struct bg_gattdb_def bg_gattdb_data;
//
//// Flag for indicating DFU Reset must be performed
//uint8_t boot_to_dfu = 0;
//
//const gecko_configuration_t config =
//{
//  .bluetooth.max_connections = MAX_CONNECTIONS,
//  .bluetooth.max_advertisers = MAX_ADVERTISERS,
//  .bluetooth.heap = bluetooth_stack_heap,
//  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
//  .bluetooth.sleep_clock_accuracy = 100,
//  .bluetooth.linklayer_priorities = &linklayer_priorities,
//  .gattdb = &bg_gattdb_data,
//  .btmesh_heap_size = BTMESH_HEAP_SIZE,
//#if (HAL_PA_ENABLE)
//  .pa.config_enable = 1, // Set this to be a valid PA config
//#if defined(FEATURE_PA_INPUT_FROM_VBAT)
//  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
//#else
//  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
//#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
//#endif // (HAL_PA_ENABLE)
//  .max_timers = 16,
//};
//
//void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);
//void mesh_native_bgapi_init(void);
//bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
//
///**
// * See light switch app.c file definition
// */
//void gecko_bgapi_classes_init_server_friend(void)
//{
//	gecko_bgapi_class_dfu_init();
//	gecko_bgapi_class_system_init();
//	gecko_bgapi_class_le_gap_init();
//	gecko_bgapi_class_le_connection_init();
//	//gecko_bgapi_class_gatt_init();
//	gecko_bgapi_class_gatt_server_init();
//	gecko_bgapi_class_hardware_init();
//	gecko_bgapi_class_flash_init();
//	gecko_bgapi_class_test_init();
//	//gecko_bgapi_class_sm_init();
//	//mesh_native_bgapi_init();
//	gecko_bgapi_class_mesh_node_init();
//	//gecko_bgapi_class_mesh_prov_init();
//	gecko_bgapi_class_mesh_proxy_init();
//	gecko_bgapi_class_mesh_proxy_server_init();
//	//gecko_bgapi_class_mesh_proxy_client_init();
//	//gecko_bgapi_class_mesh_generic_client_init();
//	gecko_bgapi_class_mesh_generic_server_init();
//	//gecko_bgapi_class_mesh_vendor_model_init();
//	//gecko_bgapi_class_mesh_health_client_init();
//	//gecko_bgapi_class_mesh_health_server_init();
//	//gecko_bgapi_class_mesh_test_init();
//	gecko_bgapi_class_mesh_lpn_init();
//	//gecko_bgapi_class_mesh_friend_init();
//}
//
//
///**
// * See main function list in soc-btmesh-switch project file
// */
//void gecko_bgapi_classes_init_client_lpn(void)
//{
//	gecko_bgapi_class_dfu_init();
//	gecko_bgapi_class_system_init();
//	gecko_bgapi_class_le_gap_init();
//	gecko_bgapi_class_le_connection_init();
//	//gecko_bgapi_class_gatt_init();
//	gecko_bgapi_class_gatt_server_init();
//	gecko_bgapi_class_hardware_init();
//	gecko_bgapi_class_flash_init();
//	gecko_bgapi_class_test_init();
//	//gecko_bgapi_class_sm_init();
//	//mesh_native_bgapi_init();
//	gecko_bgapi_class_mesh_node_init();
//	//gecko_bgapi_class_mesh_prov_init();
//	gecko_bgapi_class_mesh_proxy_init();
//	gecko_bgapi_class_mesh_proxy_server_init();
//	//gecko_bgapi_class_mesh_proxy_client_init();
//	gecko_bgapi_class_mesh_generic_client_init();
//	//gecko_bgapi_class_mesh_generic_server_init();
//	//gecko_bgapi_class_mesh_vendor_model_init();
//	//gecko_bgapi_class_mesh_health_client_init();
//	//gecko_bgapi_class_mesh_health_server_init();
//	//gecko_bgapi_class_mesh_test_init();
//	gecko_bgapi_class_mesh_lpn_init();
//	//gecko_bgapi_class_mesh_friend_init();
//
//}
//void gecko_main_init()
//{
//  // Initialize device
//  initMcu();
//  // Initialize board
//  initBoard();
//  // Initialize application
//  initApp();
//
//  // Minimize advertisement latency by allowing the advertiser to always
//  // interrupt the scanner.
//  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;
//
//  gecko_stack_init(&config);
//
//  if( DeviceUsesClientModel() ) {
//	  gecko_bgapi_classes_init_client_lpn();
//  } else {
//	  gecko_bgapi_classes_init_server_friend();
//  }
//
//  // Initialize coexistence interface. Parameters are taken from HAL config.
//  gecko_initCoexHAL();
//
//}
//
//
//void set_device_name(bd_addr *addr)
//{
//	char name[20];
//	if(DeviceUsesServerModel())
//		sprintf(name,"5823Sub %02x:%02x", addr->addr[1], addr->addr[0]);
//	else if(DeviceUsesClientModel())
//		sprintf(name,"5823Pub %02x:%02x", addr->addr[1], addr->addr[0]);
//
//	LOG_INFO("Device name = %s\n", name);
//
//	gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name);
//}
//
//
//void level_update_publish(bool button_state)
//{
////	struct mesh_generic_state pub_data;
////
////	pub_data.kind = mesh_generic_state_on_off;
////	pub_data.on_off.on = data;
//
//	transaction_id +=1;
//	struct mesh_generic_request custom_pub;
//	custom_pub.kind = mesh_generic_request_on_off;
//	custom_pub.on_off = button_state;
//	int result = mesh_lib_generic_client_publish(MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID, index, transaction_id, &custom_pub, false,false,false);
//
//	if(result)
//		LOG_INFO("Error in publishing %x\n",result);
//	else
//		LOG_INFO("Publish success\n");
//}

/* Referenced the Silicon Labs bluetooth mesh light and switch example */
//void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt)
//{
////	EMU_EnterEM1();
//  uint16_t result;
//  switch (evt_id) {
//    case gecko_evt_system_boot_id:
//    	LOG_INFO("Booted\n");
//      if(GPIO_PinInGet(PB0_PORT, PB0_PIN ) == 0 || GPIO_PinInGet(PB1_PORT, PB1_PIN ) == 0)
//      {
//    	  // Erase persistent storage
//    	  gecko_cmd_flash_ps_erase_all();
//
//    	  LOG_INFO("Factory Reset\n");
//    	  displayPrintf(DISPLAY_ROW_ACTION,"Factory Reset");
//
//    	  // Wait for 2 seconds
//    	  gecko_cmd_hardware_set_soft_timer(2 * 32768, FACTORY_RESET_ID, 1);
//      }
//
//      else
//      {
//    	  struct gecko_msg_system_get_bt_address_rsp_t *gecko_bt_addr = gecko_cmd_system_get_bt_address();
//
//    	  gecko_bt_addr = gecko_cmd_system_get_bt_address();
//
//		 /* Display server bluettoth address */
//		 displayPrintf(DISPLAY_ROW_BTADDR,"%02x:%02x:%02x:%02x:%02x:%02x", gecko_bt_addr->address.addr[5],
//																		   gecko_bt_addr->address.addr[4],
//																		   gecko_bt_addr->address.addr[3],
//																		   gecko_bt_addr->address.addr[2],
//																		   gecko_bt_addr->address.addr[1],
//																		   gecko_bt_addr->address.addr[0]);
//
//    	  if(DeviceUsesServerModel())
//    	  {
//    		  LOG_INFO("Subscriber\n");
//			  displayPrintf(DISPLAY_ROW_NAME, "Subscriber");
//    	  }
//
//    	  else if(DeviceUsesClientModel())
//    	  {
//    		  LOG_INFO("Publisher\n");
//
//			  displayPrintf(DISPLAY_ROW_NAME, "Publisher");
//    	  }
//    	  set_device_name(&gecko_bt_addr->address);
//
//    	  // Initialize Mesh stack in Node operation mode, wait for initialized event
//    	  gecko_cmd_mesh_node_init();
//
//
//      }
//      break;
//
//    case gecko_evt_hardware_soft_timer_id:
//    	LOG_INFO(" Handler\n");
//    	switch (evt->data.evt_hardware_soft_timer.handle) {
//
//    	case FACTORY_RESET_ID:
//    		displayPrintf(DISPLAY_ROW_ACTION,"Factory Reset");
//    		LOG_INFO("Reset handler\n");
//    		// Resetting device to complete factory reset
//    		gecko_cmd_system_reset(0);
//    		break;
//    	case TIMER_RESTART_ID:
//
//    		gecko_cmd_system_reset(0);
//    		break;
//    	}
//    	break;
//
//    case gecko_evt_mesh_node_initialized_id:
//
//      LOG_INFO("Node Initialized\n");
//
//      if (evt->data.evt_mesh_node_initialized.provisioned && DeviceUsesServerModel()) {
//    	  result = gecko_cmd_mesh_generic_server_init()->result;
//
//    	  if(result)
//    		  LOG_INFO("Mesh server init failed with code %d\n", result);
//      }
//
//      if (evt->data.evt_mesh_node_initialized.provisioned && DeviceUsesClientModel()) {
//
//    	  	  PB0_interrupt_enable();
//
//    	  	  result = gecko_cmd_mesh_generic_client_init()->result;
//
//         	  if(result)
//         		  LOG_INFO("Mesh client init failed with code %d", result);
//      }
//
//      if(evt->data.evt_mesh_node_initialized.provisioned && DeviceIsOnOffPublisher())
//      {
//    	  LOG_INFO("mesh_lib_init");
//    	  mesh_lib_init(malloc,free,8);
//      }
//
//      if(evt->data.evt_mesh_node_initialized.provisioned && DeviceIsOnOffSubscriber())
//      {
//    	  LOG_INFO("mesh_lib_init");
//    	  mesh_lib_init(malloc,free,9);
////    	  mesh_lib_generic_server_register_handler();
////    	  mesh_lib_generic_server_update();
////    	  mesh_lib_generic_server_publish();
//      }
//
//      if (!evt->data.evt_mesh_node_initialized.provisioned) {
//        // The Node is now initialized, start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
//        gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
//        displayPrintf(DISPLAY_ROW_ACTION,"unprovisioned");
//      }
//
//      if(evt->data.evt_mesh_node_initialized.provisioned)
//      {
//    	  displayPrintf(DISPLAY_ROW_ACTION,"provisioned");
//      }
//
//      break;
//
//    case gecko_evt_mesh_node_provisioning_started_id:
//    	displayPrintf(DISPLAY_ROW_ACTION,"provisioning");
//    	LOG_INFO("provisioning\n");
//    	break;
//
//    case gecko_evt_mesh_node_provisioned_id:
//    	SLEEP_SleepBlockBegin(sleepEM3);
//    	displayPrintf(DISPLAY_ROW_ACTION,"provisioned");
//    	LOG_INFO("provisioned\n");
//
//    	gecko_cmd_hardware_set_soft_timer(32768*2, TIMER_RESTART_ID, 1);
//    	break;
//
//    case gecko_evt_mesh_node_provisioning_failed_id:
//    	displayPrintf(DISPLAY_ROW_ACTION,"provision fail");
//    	LOG_INFO("provision fail\n");
//    	break;
//
//    case gecko_evt_mesh_generic_server_client_request_id:
//    	if(DeviceUsesServerModel())
//    	{
//    		uint8_t button_press;
//    		button_press = evt->data.evt_mesh_generic_server_client_request.parameters.data[0];
//    		LOG_INFO("MESH BUTTON SUB STATE = %d", button_press);
////    		mesh_lib_generic_server_event_handler(evt);
//
//    		if(button_press == 1)
//			{
//				displayPrintf(DISPLAY_ROW_TEMPVALUE, "Button Pressed");
//			}
//			else
//			{
//				displayPrintf(DISPLAY_ROW_TEMPVALUE, "Button Released");
//			}
//    	}
//    	break;
//
//    case gecko_evt_mesh_generic_server_state_changed_id:
//    	if(DeviceUsesServerModel())
//		{
//    		mesh_lib_generic_server_event_handler(evt);
//		}
//    	break;
//
//    case gecko_evt_le_connection_opened_id:
//    	LOG_INFO("Connected");
//    	displayPrintf(DISPLAY_ROW_CONNECTION,"connected");
//    	break;
//
//    case gecko_evt_le_connection_closed_id:
//      /* Check if need to boot to dfu mode */
//      if (boot_to_dfu) {
//        /* Enter to DFU OTA mode */
//        gecko_cmd_system_reset(2);
//      }
//      break;
//
//    case gecko_evt_mesh_node_reset_id:
//    	gecko_cmd_flash_ps_erase_all();
//
//    	LOG_INFO("Factory Reset 2\n");
//
//	    // Wait for 2 seconds
//	    gecko_cmd_hardware_set_soft_timer(2 * 32768, FACTORY_RESET_ID, 1);
//    	break;
//
//    case gecko_evt_gatt_server_user_write_request_id:
//      if (evt->data.evt_gatt_server_user_write_request.characteristic == gattdb_ota_control) {
//        /* Set flag to enter to OTA mode */
//        boot_to_dfu = 1;
//        /* Send response to Write Request */
//        gecko_cmd_gatt_server_send_user_write_response(
//          evt->data.evt_gatt_server_user_write_request.connection,
//          gattdb_ota_control,
//          bg_err_success);
//
//        /* Close connection to enter to DFU OTA mode */
//        gecko_cmd_le_connection_close(evt->data.evt_gatt_server_user_write_request.connection);
//      }
//      break;
//
//    case gecko_evt_system_external_signal_id:
//    	if((evt->data.evt_system_external_signal.extsignals & DISP_INT_MASK))
//		{
//    		CORE_AtomicDisableIrq();
//			interrupt_flags_set &= ~(DISP_INT_MASK); // Disable COMP1 Interrupt bit mask
//			CORE_AtomicEnableIrq();
//			displayUpdate();
//			LOG_DEBUG("Display update call");
//		}
//
//    	if((DeviceIsOnOffPublisher())&&(evt->data.evt_system_external_signal.extsignals & BUTTON_INT_MASK) != 0)
//		{
//			CORE_AtomicDisableIrq();
//			interrupt_flags_set &= ~(BUTTON_INT_MASK); // Disable Button PB0 Interrupt bit mask
//			CORE_AtomicEnableIrq();
//
//			bool value = !pin_state;
//
////			i2c_read(0X00,1);
//
//			LOG_INFO("Button state = %d\n",value);
//
//			level_update_publish(value);
//		}
//
//    	if((DeviceIsOnOffPublisher())&&(evt->data.evt_system_external_signal.extsignals & FALL_INT_MASK) != 0)
//		{
////    		EMU_EnterEM1();
//			CORE_AtomicDisableIrq();
//			interrupt_flags_set &= ~(FALL_INT_MASK); // Disable Fall Interrupt bit mask
//			CORE_AtomicEnableIrq();
//
//			bool value = fall_state;
//
//			LOG_INFO("Fall state = %d\n",value);
//
////			i2c_read(0X16,1);
//
//			LOG_INFO("READ RESULT = %x\n", data_array[0]);
//
////			EMU_EnterEM2(false);
////			level_update_publish(value);
////			EMU_EnterEM2(false);
//		}
//    	break;
//
//    default:
//      break;
//  }
//
//  EMU_EnterEM2(false);
//}
