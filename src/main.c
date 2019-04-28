/*
 * File : main.c
 *
 *  Created on: Apr 13, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: Silicon labs light switch example mesh implementation
 */
#include "main.h"



uint32_t interrupt_flags_set;
const int lowest_sleep_mode = 0; // Setting the lowest sleep mode
uint8_t pin_state = 1; // Variable to read PB0 button state
uint8_t fall_state; // Variable to store patient fall state
uint8_t tap_state; // Variable to store patient tap state

uint8_t tap_config_button;

static uint8 num_connections = 0;

volatile uint8_t pin_pressed_flag = 0;

bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);

// bluetooth stack heap
#define MAX_CONNECTIONS 2

uint8_t bluetooth_stack_heap[DEFAULT_BLUETOOTH_HEAP(MAX_CONNECTIONS) + BTMESH_HEAP_SIZE + 1760];

// Bluetooth advertisement set configuration
//
// At minimum the following is required:
// * One advertisement set for Bluetooth LE stack (handle number 0)
// * One advertisement set for Mesh data (handle number 1)
// * One advertisement set for Mesh unprovisioned beacons (handle number 2)
// * One advertisement set for Mesh unprovisioned URI (handle number 3)
// * N advertisement sets for Mesh GATT service advertisements
// (one for each network key, handle numbers 4 .. N+3)
//
#define MAX_ADVERTISERS (4 + MESH_CFG_MAX_NETKEYS)

extern uint8_t pin_state;
extern uint8_t fall_state;


uint16_t index = 0x00;

uint8_t transaction_id = 0;

static gecko_bluetooth_ll_priorities linklayer_priorities = GECKO_BLUETOOTH_PRIORITIES_DEFAULT;

// bluetooth stack configuration
extern const struct bg_gattdb_def bg_gattdb_data;

// Flag for indicating DFU Reset must be performed
uint8_t boot_to_dfu = 0;

static uint8 lpn_active = 0;

static uint16 _elem_index = 0xffff;

const gecko_configuration_t config =
{
  .sleep.flags = SLEEP_FLAGS_DEEP_SLEEP_ENABLE,
  .bluetooth.max_connections = MAX_CONNECTIONS,
  .bluetooth.max_advertisers = MAX_ADVERTISERS,
  .bluetooth.heap = bluetooth_stack_heap,
  .bluetooth.heap_size = sizeof(bluetooth_stack_heap) - BTMESH_HEAP_SIZE,
  .bluetooth.sleep_clock_accuracy = 100,
  .bluetooth.linklayer_priorities = &linklayer_priorities,
  .gattdb = &bg_gattdb_data,
  .btmesh_heap_size = BTMESH_HEAP_SIZE,
#if (HAL_PA_ENABLE)
  .pa.config_enable = 1, // Set this to be a valid PA config
#if defined(FEATURE_PA_INPUT_FROM_VBAT)
  .pa.input = GECKO_RADIO_PA_INPUT_VBAT, // Configure PA input to VBAT
#else
  .pa.input = GECKO_RADIO_PA_INPUT_DCDC,
#endif // defined(FEATURE_PA_INPUT_FROM_VBAT)
#endif // (HAL_PA_ENABLE)
  .max_timers = 16,
};


/**
--------------------------------------------------------------------------------------------
gecko_bgapi_classes_init_server_lpn
--------------------------------------------------------------------------------------------
*	This function works as the initialization for lpn as server
*
* 	@\param			void
*
* 	@\return		void
*
* 	Reference : Function list in soc-btmesh-switch project file
*
**/
void gecko_bgapi_classes_init_server_lpn(void)
{
	gecko_bgapi_class_dfu_init();
	gecko_bgapi_class_system_init();
	gecko_bgapi_class_le_gap_init();
	gecko_bgapi_class_le_connection_init();
	//gecko_bgapi_class_gatt_init();
	gecko_bgapi_class_gatt_server_init();
	gecko_bgapi_class_hardware_init();
	gecko_bgapi_class_flash_init();
	gecko_bgapi_class_test_init();
	//gecko_bgapi_class_sm_init();
	//mesh_native_bgapi_init();


	gecko_bgapi_class_mesh_node_init();
	//gecko_bgapi_class_mesh_prov_init();
	gecko_bgapi_class_mesh_proxy_init();
	gecko_bgapi_class_mesh_proxy_server_init();
	//gecko_bgapi_class_mesh_proxy_client_init();
//	gecko_bgapi_class_mesh_generic_client_init();
	gecko_bgapi_class_mesh_generic_server_init();
	//gecko_bgapi_class_mesh_vendor_model_init();
	//gecko_bgapi_class_mesh_health_client_init();
	//gecko_bgapi_class_mesh_health_server_init();
	//gecko_bgapi_class_mesh_test_init();
	gecko_bgapi_class_mesh_lpn_init();
	//gecko_bgapi_class_mesh_friend_init();

}


/**
--------------------------------------------------------------------------------------------
gecko_main_init
--------------------------------------------------------------------------------------------
*	This function works as the initialization for the board and mesh stack
*
* 	@\param			void
*
* 	@\return		void
*
* 	Reference : Function list in soc-btmesh-switch project file
*
**/
void gecko_main_init()
{
  // Initialize device
  initMcu();
  // Initialize board
  initBoard();
  // Initialize application
  initApp();

  // Minimize advertisement latency by allowing the advertiser to always
  // interrupt the scanner.
  linklayer_priorities.scan_max = linklayer_priorities.adv_min + 1;

  gecko_stack_init(&config);

  gecko_bgapi_classes_init_server_lpn();

  // Initialize coexistence interface. Parameters are taken from HAL config.
  gecko_initCoexHAL();

}

/**
--------------------------------------------------------------------------------------------
lpn_init
--------------------------------------------------------------------------------------------
*	This function works as the initialization for lpn
*
* 	@\param			void
*
* 	@\return		void
*
* 	Reference : Function list in soc-btmesh-switch project file
*
**/
void lpn_init(void)
{
  uint16 result;

  // Do not initialize LPN if lpn is currently active
  // or any GATT connection is opened
  if (lpn_active || num_connections) {
    return;
  }

  // Initialize LPN functionality.
  result = gecko_cmd_mesh_lpn_init()->result;
  if (result) {
	  LOG_ERROR("LPN init failed (0x%x)\r\n", result);
    return;
  }
  lpn_active = 1;
  LOG_INFO("LPN initialized\r\n");

  // Configure the lpn with following parameters:
  // - Minimum friend queue length = 2
  // - Poll timeout = 5 seconds
  result = gecko_cmd_mesh_lpn_configure(2, 5 * 1000)->result;
  if (result) {
	  LOG_ERROR("LPN conf failed (0x%x)\r\n", result);
    return;
  }

  LOG_INFO("trying to find friend...\r\n");
  result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

  if (result != 0) {
	LOG_DEBUG("ret.code %x\r\n", result);
  }
}


/**
--------------------------------------------------------------------------------------------
lpn_deinit
--------------------------------------------------------------------------------------------
*	This function works as the de-initialization for lpn
*
* 	@\param			void
*
* 	@\return		void
*
* 	Reference : Function list in soc-btmesh-switch project file
*
**/
void lpn_deinit(void)
{
  uint16 result;

  if (!lpn_active) {
    return; // lpn feature is currently inactive
  }

  result = gecko_cmd_hardware_set_soft_timer(0, // cancel friend finding timer
                                             TIMER_ID_FRIEND_FIND,
                                             1)->result;

  // Terminate friendship if exist
  result = gecko_cmd_mesh_lpn_terminate_friendship()->result;
  if (result) {
    LOG_ERROR("Friendship termination failed (0x%x)\r\n", result);
  }
  // turn off lpn feature
  result = gecko_cmd_mesh_lpn_deinit()->result;
  if (result) {
	  LOG_ERROR("LPN deinit failed (0x%x)\r\n", result);
  }
  lpn_active = 0;
  LOG_INFO("LPN deinitialized\r\n");
 }


/**
--------------------------------------------------------------------------------------------
set_device_name
--------------------------------------------------------------------------------------------
*	This function sets the device name string
*
* 	@\param			addr
*
* 	@\return		void
*
*
**/
void set_device_name(bd_addr *addr)
{
	char name[20];
	if(DeviceUsesServerModel())
		sprintf(name,"5823Sub %02x:%02x", addr->addr[1], addr->addr[0]);
	else if(DeviceUsesClientModel())
		sprintf(name,"5823Pub %02x:%02x", addr->addr[1], addr->addr[0]);

	LOG_DEBUG("Device name = %s\n", name);

	gecko_cmd_gatt_server_write_attribute_value(gattdb_device_name, 0, strlen(name), (uint8 *)name);
}

/**
--------------------------------------------------------------------------------------------
level_update_publish
--------------------------------------------------------------------------------------------
*	This function works as the level publish function
*
* 	@\param			button_state
*
* 	@\return		void
*
*
**/
void level_update_publish(int8_t level_state)
{

	struct mesh_generic_state custom_pub;
	custom_pub.kind = mesh_generic_state_level; // Set publish model as level
	custom_pub.level.level = level_state;

	LOG_DEBUG("INSIDE PUBLISHER\n");
	int result;

	// Perform server update
	result = mesh_lib_generic_server_update(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,	_elem_index, &custom_pub, 0,0);

	LOG_DEBUG("RESULT = %d\n", result);
	if(result)
	{
		LOG_ERROR("Error is publish update = %d\n", result);
	}

	else
	{
		LOG_DEBUG("INSIDE PUBLISHER 1\n");
		//Perform server publish
		result = mesh_lib_generic_server_publish(MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
														_elem_index,
														mesh_generic_state_level);

		if(result)
		{
			LOG_ERROR("Error is publish\n");
		}
		else
		{
			LOG_DEBUG("Publish success\n");
		}
	}

}

/**
--------------------------------------------------------------------------------------------
gecko_event_handler
--------------------------------------------------------------------------------------------
*	This function works as the event handler for the mesh stack
*
* 	@\param			evt_id, evt
*
* 	@\return		void
*
* 	Reference : Function list in soc-btmesh-switch project file
*
**/

void gecko_event_handler(uint32_t evt_id, struct gecko_cmd_packet *evt)
{
  uint16_t result;
  uint8_t friend_data;
  switch (evt_id) {
    case gecko_evt_system_boot_id:
      LOG_DEBUG("Booted\n");

      displayPrintf(DISPLAY_ROW_NAME,"Low Power Node");
   	  displayPrintf(DISPLAY_ROW_BTADDR2,"Patient Monitor");

   	  if(GPIO_PinInGet(PB0_PORT, PB0_PIN ) == 0 || GPIO_PinInGet(PB1_PORT, PB1_PIN ) == 0) //Factory reset condition
      {
    	  // Erase persistent storage
   		  BTSTACK_CHECK_RESPONSE(gecko_cmd_flash_ps_erase_all());

    	  LOG_INFO("Factory Reset\n");

    	  displayPrintf(DISPLAY_ROW_ACTION,"Factory Reset");

    	  // Wait for 2 seconds
    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(2 * 32768, FACTORY_RESET_ID, 1));
      }

      else
      {
    	  struct gecko_msg_system_get_bt_address_rsp_t *gecko_bt_addr = gecko_cmd_system_get_bt_address();

    	  gecko_bt_addr = gecko_cmd_system_get_bt_address();

		  /* Display server bluettoth address */
		  displayPrintf(DISPLAY_ROW_BTADDR,"%02x:%02x:%02x:%02x:%02x:%02x", gecko_bt_addr->address.addr[5],
																		   gecko_bt_addr->address.addr[4],
																		   gecko_bt_addr->address.addr[3],
																		   gecko_bt_addr->address.addr[2],
																		   gecko_bt_addr->address.addr[1],
																		   gecko_bt_addr->address.addr[0]);

    	  set_device_name(&gecko_bt_addr->address);

    	  // Initialize Mesh stack in Node operation mode, wait for initialized event
    	  gecko_cmd_mesh_node_init();


      }
      break;

    case gecko_evt_hardware_soft_timer_id:
    	switch (evt->data.evt_hardware_soft_timer.handle) {

    	case FACTORY_RESET_ID:
    		displayPrintf(DISPLAY_ROW_ACTION,"Factory Reset");
    		LOG_DEBUG("Reset handler\n");
    		// Resetting device to complete factory reset
    		gecko_cmd_system_reset(0);
    		break;
    	case TIMER_RESTART_ID:

    		gecko_cmd_system_reset(0);
    		break;

    	case TIMER_ID_FRIEND_FIND:
			{
			  LOG_INFO("trying to find friend...\r\n");
			  result = gecko_cmd_mesh_lpn_establish_friendship(0)->result;

			  if (result != 0) {
				LOG_ERROR("ret.code %x\r\n", result);
			  }
			}
			break;

    	case DISPLAY_REFRESH:
    		timer_count+=1;
    		displayUpdate();
    		break;
    	}
    	break;

    case gecko_evt_mesh_node_initialized_id:

      LOG_INFO("Node Initialized\n");

      result = gecko_cmd_mesh_generic_server_init()->result;

      if(result)
    	  LOG_ERROR("Mesh server init failed with code %d\n", result);



      if (!evt->data.evt_mesh_node_initialized.provisioned) {
        // The Node is now initialized, start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
        gecko_cmd_mesh_node_start_unprov_beaconing(0x3);
        displayPrintf(DISPLAY_ROW_ACTION,"unprovisioned");
      }

      struct gecko_msg_mesh_node_initialized_evt_t *pData = (struct gecko_msg_mesh_node_initialized_evt_t *)&(evt->data);

      if(evt->data.evt_mesh_node_initialized.provisioned)
      {
    	  // Load data from persistent storage
    	  if(ps_load(PS_KEY_FALL_CONFIGURED, &is_fall_configured, sizeof(is_fall_configured)) == 0)
    	  {
    		  LOG_DEBUG("PS LOAD of fall config loaded successfully\n");
    	  }
    	  else
    	  {
    		  LOG_DEBUG("PS LOAD of fall config load failed\n");
    	  }
		  if(ps_load(PS_KEY_TAP_CONFIGURED, &is_tap_configured, sizeof(is_tap_configured)) == 0)
		  {
			  LOG_DEBUG("PS LOAD of tap config loaded successfully\n");
		  }
		  else
		  {
			  LOG_DEBUG("PS LOAD of tap config load failed\n");
		  }
		  if(ps_load(PS_KEY_BUZZER_STATE, &is_buzzer_on, sizeof(is_buzzer_on)) == 0)
		  {
			  LOG_DEBUG("PS LOAD of buzzer state loaded successfully\n");
		  }
		  else
		  {
			  LOG_DEBUG("PS LOAD of buzzer state load failed\n");
		  }

		  // Check persistent storage on provisioning to enable required modes
		  if(is_fall_configured)
		  {
			  accel_config_freefall();
			  displayPrintf(DISPLAY_ROW_CONNECTION,"Fall configured");
		  }
		  else
		  {
			  displayPrintf(DISPLAY_ROW_CONNECTION,"PB0 - Fall config ");
		  }

		  if(is_tap_configured)
		  {
			  accel_config_tap();
			  displayPrintf(DISPLAY_ROW_CLIENTADDR,"Tap configured");
		  }
		  else
		  {
			  displayPrintf(DISPLAY_ROW_CLIENTADDR,"PB1 - Tap config");
		  }

		  if(is_buzzer_on)
		  {
			  displayPrintf(DISPLAY_ROW_PASSKEY,"Buzzer ON");
			  gpioLed1SetOn();
			  LOG_DEBUG("BUZZER ON");
		  }
		  else
		  {
			  displayPrintf(DISPLAY_ROW_PASSKEY,"Buzzer OFF");
			  LOG_DEBUG("BUZZER OFF\n");
			  gpioLed1SetOff();
		  }

		  LOG_DEBUG("FALL CONFIGURED = %d\n", is_fall_configured);

		  LOG_DEBUG("TAP CONFIGURED = %d\n", is_tap_configured);

		  LOG_DEBUG("BUTTON STATE = %d\n", is_buzzer_on);
    	  LOG_INFO("LPN PROVISIONED ADDRESS : %x\n", pData->address);
    	  displayPrintf(DISPLAY_ROW_TEMPVALUE,"Provision address = %x", pData->address);
    	  _elem_index = 0;

    	  mesh_lib_init(malloc, free, 10); // upto 10 models can be added in the .isc
    	  lpn_init();

    	  displayPrintf(DISPLAY_ROW_ACTION,"provisioned");

    	  interrupt_enable();

      }

      break;

    case gecko_evt_mesh_node_provisioning_started_id:
    	displayPrintf(DISPLAY_ROW_ACTION,"provisioning");
    	LOG_INFO("provisioning\n");
    	break;

    case gecko_evt_mesh_node_provisioned_id:

    	displayPrintf(DISPLAY_ROW_ACTION,"provisioned");

    	_elem_index = 0;

    	mesh_lib_init(malloc, free, 10);

    	LOG_INFO("provisioned\n");

    	lpn_init();
    	break;

    case gecko_evt_mesh_node_provisioning_failed_id:
    	displayPrintf(DISPLAY_ROW_ACTION,"provision fail");
    	BTSTACK_CHECK_RESPONSE(gecko_cmd_hardware_set_soft_timer(2*32768, TIMER_RESTART_ID ,1));
    	LOG_ERROR("provision fail\n");
    	break;

    case gecko_evt_mesh_generic_server_client_request_id:

    	friend_data = evt->data.evt_mesh_generic_server_client_request.parameters.data[0];
		LOG_DEBUG("DATA RECEIVED STATE = %d\n", friend_data);


		if(friend_data == 1)
		{
			is_buzzer_on = 0;
			ps_save(PS_KEY_BUZZER_STATE, &is_buzzer_on, sizeof(is_buzzer_on));
			LOG_WARN("\nTURN ALARM OFF\n");
			displayPrintf(DISPLAY_ROW_PASSKEY,"Buzzer OFF");
			gpioLed1SetOff();
		}


    	break;

    case gecko_evt_mesh_generic_server_state_changed_id:
    	LOG_DEBUG("Server state changed\n");
    	mesh_lib_generic_server_event_handler(evt);
    	break;

    case gecko_evt_le_connection_opened_id:
    	LOG_DEBUG("Connected");
    	num_connections++;
    	displayPrintf(DISPLAY_ROW_CONNECTION,"connected");
    	break;

    case gecko_evt_le_connection_closed_id:
      /* Check if need to boot to dfu mode */
      if (boot_to_dfu) {
        /* Enter to DFU OTA mode */
        gecko_cmd_system_reset(2);
      }
      if (num_connections > 0) {
              if (--num_connections == 0) {
            	 lpn_init();
              }
      }
      break;

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

    case gecko_evt_mesh_lpn_friendship_established_id:
          LOG_INFO("friendship established\r\n");
          displayPrintf(DISPLAY_ROW_ACTION,"Friend Established");
      break;

    case gecko_evt_mesh_lpn_friendship_failed_id:
	  LOG_ERROR("friendship failed\r\n");
	  displayPrintf(DISPLAY_ROW_ACTION,"Friendship Failed");
	  // try again in 2 seconds
	  result = gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000),
												 TIMER_ID_FRIEND_FIND,
												 1)->result;
	  if (result) {
		LOG_ERROR("timer failure?!  %x\r\n", result);
	  }
	  break;

	case gecko_evt_mesh_lpn_friendship_terminated_id:
	  LOG_INFO("friendship terminated\r\n");
	  displayPrintf(DISPLAY_ROW_ACTION,"Friend Term");
	  if (num_connections == 0) {
		// try again in 2 seconds
		result = gecko_cmd_hardware_set_soft_timer(TIMER_MS_2_TIMERTICK(2000),
												   TIMER_ID_FRIEND_FIND,
												   1)->result;
		if (result) {
		  LOG_ERROR("timer failure?!  %x\r\n", result);
		}
	  }
	  break;

    case gecko_evt_system_external_signal_id:
    	// External event handler to configure fall detection mode
    	if((evt->data.evt_system_external_signal.extsignals & FALL_CONFIG_BUTTON) != 0)
		{
			CORE_AtomicDisableIrq();
			interrupt_flags_set &= ~(FALL_CONFIG_BUTTON); // Disable Button PB0 Interrupt bit mask
			CORE_AtomicEnableIrq();

			if(accel_config_freefall() != 0)
			{
				LOG_ERROR("Failed in Initializing free fall mode\n");
				is_fall_configured = 0;
				displayPrintf(DISPLAY_ROW_CLIENTADDR,"PB1 - Tap config");
				displayPrintf(DISPLAY_ROW_CONNECTION,"Fall configure failed");
			}
			else
			{
				is_fall_configured = 1;
				is_tap_configured =0;

				displayPrintf(DISPLAY_ROW_CLIENTADDR,"PB1 - Tap config");
				displayPrintf(DISPLAY_ROW_CONNECTION,"Fall configured");
			}

			ps_save(PS_KEY_FALL_CONFIGURED, &is_fall_configured, sizeof(is_fall_configured));
			ps_save(PS_KEY_TAP_CONFIGURED, &is_tap_configured, sizeof(is_tap_configured));
		}
    	// External event handler to configure tap detection mode
    	if((evt->data.evt_system_external_signal.extsignals & TAP_CONFIG_BUTTON) != 0)
		{
			CORE_AtomicDisableIrq();
			interrupt_flags_set &= ~(TAP_CONFIG_BUTTON); // Disable Button PB0 Interrupt bit mask
			CORE_AtomicEnableIrq();

			bool value = !tap_config_button;

			if(accel_config_tap() != 0)
			{
			  LOG_ERROR("Failed in Initializing tap mode\n");
			  is_tap_configured = 0;
			  displayPrintf(DISPLAY_ROW_CLIENTADDR,"Tap configure failed");
			  displayPrintf(DISPLAY_ROW_CONNECTION,"PB0 - Fall config");
			}
			else
			{
				is_tap_configured = 1;
				is_fall_configured = 0;

				displayPrintf(DISPLAY_ROW_CLIENTADDR,"Tap configured");
				displayPrintf(DISPLAY_ROW_CONNECTION,"PB0 - Fall config");
			}

			LOG_DEBUG("Tap Button state = %d\n",value);
			ps_save(PS_KEY_TAP_CONFIGURED, &is_tap_configured, sizeof(is_tap_configured));
			ps_save(PS_KEY_FALL_CONFIGURED, &is_fall_configured, sizeof(is_fall_configured));
    	}

    	// External event handler to publish fall detection
    	if((evt->data.evt_system_external_signal.extsignals & FALL_INT_MASK) && (is_fall_configured) != 0)
		{
			CORE_AtomicDisableIrq();
			interrupt_flags_set &= ~(FALL_INT_MASK); // Disable Fall Interrupt bit mask
			CORE_AtomicEnableIrq();

			bool value = fall_state;

			LOG_DEBUG("Fall state = %d\n",value);


			i2c_read(0x16, 1);

			displayPrintf(DISPLAY_ROW_PASSKEY,"Buzzer ON");
			gpioLed1SetOn();

			LOG_WARN("\nSOUNDING ALARM FOR FALL DETECTION\n");
			level_update_publish(40);	// Publish level 40 signaling fall detected

		}
    	// External event handler to publish tap detection
    	if((evt->data.evt_system_external_signal.extsignals & TAP_INT_MASK) && (is_tap_configured) != 0)
		{
			CORE_AtomicDisableIrq();
			interrupt_flags_set &= ~(TAP_INT_MASK); // Disable Fall Interrupt bit mask
			CORE_AtomicEnableIrq();

			bool value = tap_state;

			LOG_DEBUG("Tap state = %d\n",value);

			i2c_read(0X22,1);

			displayPrintf(DISPLAY_ROW_PASSKEY,"Buzzer ON");
			gpioLed1SetOn();

			level_update_publish(41); // Publish level 41 signaling tap detected

			LOG_WARN("\nSOUNDING ALARM FOR TAP DETECTION\n");

		}
    	break;

    case gecko_evt_mesh_node_reset_id:
		gecko_cmd_flash_ps_erase_all();

		LOG_INFO("Factory Reset 2\n");

		// Wait for 2 seconds
		gecko_cmd_hardware_set_soft_timer(2 * 32768, FACTORY_RESET_ID, 1);
		break;

    default:
      break;
  }

}

int main(void)
{

  // Initialize stack
  gecko_main_init();

  // Initialize logger
  logInit();

  // Initialize display
  displayInit();

  // Initialize gpio
  gpioInit();


  gecko_cmd_hardware_set_soft_timer(1 * 32768, DISPLAY_REFRESH, 0); // Set repeating timer for display update and logger timestamp update

  // Initialize i2c
  if(i2c_init() != 0)
  {
	  LOG_ERROR("Failed in Initializing I2C\n");
	  return -1;
  }


  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {
		gecko_event_handler(BGLIB_MSG_ID(evt->header), evt);
	}
  }
}
