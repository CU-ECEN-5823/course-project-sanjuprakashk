#include <stdbool.h>
#include "log.h"
#include "gpio.h"
#include "letimer.h"
#include "cmu_config.h"
#include "display.h"
#include "i2c_config.h"
#include "src/ble_mesh_device_type.h"

uint32_t interrupt_flags_set;
const int lowest_sleep_mode = 0; // Setting the lowest sleep mode
uint8_t pin_state = 1; // Variable to read PB0 button state
uint8_t fall_state;

volatile uint8_t pin_pressed_flag = 0;

extern void gecko_main_init();
bool mesh_bgapi_listener(struct gecko_cmd_packet *evt);
extern void handle_gecko_event(uint32_t evt_id, struct gecko_cmd_packet *evt);

int main(void)
{

  // Initialize stack
  gecko_main_init();

  logInit();

  // Initialize CMU
   cmu_Init();

   //Initialize LETIMER0
   LETIMER0_init();

  displayInit();

  gpioInit();

  if(DeviceIsOnOffPublisher())
  {
	  i2c_init();

	  accel_config();
  }
  /* Infinite loop */
  while (1) {
	struct gecko_cmd_packet *evt = gecko_wait_event();
	bool pass = mesh_bgapi_listener(evt);
	if (pass) {

		handle_gecko_event(BGLIB_MSG_ID(evt->header), evt);
	}
  };
}
