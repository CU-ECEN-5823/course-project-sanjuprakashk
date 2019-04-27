#ifndef MAIN_H
#define MAIN_H

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


 uint8_t write_started;
 state scheduler_state;
 freefall freefall_state;

 uint8_t is_tap_configured;
 uint8_t is_fall_configured;
 uint8_t is_buzzer_on;

 #endif
