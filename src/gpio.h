/*
 * gpio.h
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */

#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_
#include <stdbool.h>
#include "native_gecko.h"
#include "gpiointerrupt.h"
#include "i2c_config.h"
#include "sleep.h"


#define LCD_PORT  gpioPortD
#define LCD_PIN	  15
#define EXTCOMIN_PORT  gpioPortD
#define EXTCOMIN_PIN   13

#define PB0_PORT	gpioPortF
#define PB0_PIN		6

#define PB1_PORT	gpioPortF
#define PB1_PIN		7

#define PC7_PORT	gpioPortC
#define PC7_PIN		7


#define GPIO_TAP_INT_PORT	gpioPortF
#define GPIO_TAP_INT_PIN	3

#define GPIO_FALL_INT_PORT	gpioPortC
#define GPIO_FALL_INT_PIN	9

#define GPIO_DISPLAY_SUPPORT_IMPLEMENTED 1
#define GPIO_SET_DISPLAY_EXT_COMIN_IMPLEMENTED 1

#define BUTTON_INT_MASK (1UL << 5)
#define FALL_INT_MASK	(1UL << 6)
#define TAP_INT_MASK 	(1UL << 7)
#define TAP_CONFIG_BUTTON (1UL << 8)

int flag;


void gpioInit();
void gpioLed0SetOn();
void gpioLed0SetOff();
void gpioLed1SetOn();
void gpioLed1SetOff();
void gpioEnableDisplay();
void gpioSetDisplayExtcomin(bool high);
void interrupt_enable();
#endif /* SRC_GPIO_H_ */
