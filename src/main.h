/*
 * File : main.h
 *
 *  Created on: Apr 13, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: Silicon labs light switch example mesh implementation
 */

#ifndef MAIN_H
#define MAIN_H

/*
 * 			INCLUDES
 */
#include <stdbool.h>
#include "letimer.h"
#include "cmu_config.h"
#include "display.h"
#include "src/ble_mesh_device_type.h"
#include "sleep.h"
#include "em_emu.h"

#include "init_app.h"
#include "init_mcu.h"
#include "init_board.h"
#include "gatt_db.h"
#include <gecko_configuration.h>
#include <mesh_sizes.h>

#include "gpio.h"
#include "log.h"
#include "display.h"
#include "letimer.h"
#include "i2c_config.h"
#include "persistent_storage.h"

#include "mesh_generic_model_capi_types.h"

#include "mesh_lib.h"
#include "gecko_ble_errors.h"

/*
 *  		MACROS
 */
#define TIMER_ID_FRIEND_FIND 20
#define TIMER_ID_NODE_CONFIGURED  30
#define TIMER_ID_PROVISIONING     66
#define FACTORY_RESET_ID 78
#define TIMER_RESTART_ID 77
#define DISPLAY_REFRESH	79

#define TIMER_CLK_FREQ ((uint32)32768)

#define TIMER_MS_2_TIMERTICK(ms) ((TIMER_CLK_FREQ * ms) / 1000)

/* Structure to store the event values */
typedef struct sch_events{
	bool tx_done;
	bool tx_err;
}sch_events;

sch_events sch_event;

typedef enum  {
 	STANDBY,
	WAIT_FOR_STANDBY_COMPLETE,
	START_FREEFALL_CONFIG,
	START_TAP_CONFIG,
	ACTIVE,
	WAIT_FOR_ACTIVE_COMPLETE,
	I2C_CONFIG_DONE,

 }state;

typedef enum  {
  	CONFIG_REG_SET,
	FREEFALL_THRESHOLD_SET,
	DEBOUNCE_COUNTER_SET,
	ENABLE_FREEFALL_INT_SYSTEM,
	WRITE_FREEFALL_INT,
	ENABLE_INT2_PIN,
	WAIT_FOR_INT2_ENABLE_COMPLETE,

 	I2C_FREEFALL_CONFIG_DONE,

 }freefall;

/*
* 		GLOBALS
*/
uint8_t write_started;
state scheduler_state;
freefall freefall_state;

uint8_t is_tap_configured;
uint8_t is_fall_configured;
uint8_t is_buzzer_on;

#endif
