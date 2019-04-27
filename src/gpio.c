/*
 * gpio.c
 *
 *  Created on: Dec 12, 2018
 *      Author: Dan Walkes
 */
#include "gpio.h"
#include "em_gpio.h"
#include <string.h>
#include "log.h"
#include <em_core.h>


#define	LED0_port gpioPortF
#define LED0_pin	4
#define LED1_port gpioPortF
#define LED1_pin 5

extern uint8_t pin_state;
extern uint8_t fall_state;
extern uint8_t tap_state;
extern uint8_t tap_config_button;

extern volatile uint8_t pin_pressed_flag;
extern uint32_t interrupt_flags_set;

void gpioInit()
{
	GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED0_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED0_port, LED0_pin, gpioModePushPull, false);
	GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateStrong);
	//GPIO_DriveStrengthSet(LED1_port, gpioDriveStrengthWeakAlternateWeak);
	GPIO_PinModeSet(LED1_port, LED1_pin, gpioModePushPull, false);

	GPIO_PinModeSet(PB1_PORT, PB1_PIN, gpioModeInput, false); // Enable button PB1

	GPIO_PinModeSet(PB0_PORT, PB0_PIN, gpioModeInput, false); // Enable button PB0

	GPIO_PinModeSet(GPIO_TAP_INT_PORT, GPIO_TAP_INT_PIN, gpioModeInput, false); // Enable button PC9
	GPIO_PinModeSet(GPIO_FALL_INT_PORT, GPIO_FALL_INT_PIN, gpioModeInput, false);

}

void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED0_port,LED0_pin);
}
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED0_port,LED0_pin);
}
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED1_port,LED1_pin);
}
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED1_port,LED1_pin);
}

void gpioEnableDisplay()
{
	GPIO_PinOutSet(LCD_PORT,LCD_PIN);
}


void gpioSetDisplayExtcomin(bool high)
{
	if(high)
	{
		GPIO_PinOutSet(EXTCOMIN_PORT,EXTCOMIN_PIN);
	}
	else
	{
		GPIO_PinOutClear(EXTCOMIN_PORT,EXTCOMIN_PIN);
	}
}

void interrupt_enable()
{
	GPIOINT_Init();

	GPIO_IntConfig(GPIO_TAP_INT_PORT, GPIO_TAP_INT_PIN, true, false, true); // Enable GPIO interrupt, rising edge triggered

	GPIO_IntEnable(1<<GPIO_TAP_INT_PIN);

	GPIO_IntConfig(GPIO_FALL_INT_PORT, GPIO_FALL_INT_PIN, true, true, true);

	GPIO_IntEnable(1<<GPIO_FALL_INT_PIN);

	GPIO_IntConfig(PB0_PORT, PB0_PIN, true, false, true); // Enable GPIO interrupt

	GPIO_IntEnable(1<<PB0_PIN);

	GPIO_IntConfig(PB1_PORT, PB1_PIN, true, false, true); // Enable GPIO interrupt

	GPIO_IntEnable(1<<PB1_PIN);
}

void GPIO_EVEN_IRQHandler()
{
	flag = GPIO_IntGet();

	pin_state = GPIO_PinInGet(PB0_PORT, PB0_PIN);

	if(!pin_state)
	{
		pin_pressed_flag = 1;
		LOG_DEBUG("State of pin pressed flag in interrupt = %d", pin_pressed_flag);
	}

	LOG_INFO("BUTTON FLAG = %d\n",flag);
	LOG_DEBUG("State of pin in interrupt = %d", pin_state);

	interrupt_flags_set |= BUTTON_INT_MASK;


	GPIO_IntClear(flag);


	gecko_external_signal(interrupt_flags_set); // Set gecko external event

}

void GPIO_ODD_IRQHandler()
{
	LOG_INFO("INSIDE INTERRUPT\n");
	flag = GPIO_IntGet();

	LOG_INFO("INT FLAG = %d\n", flag);

	if(flag == 0x200) // Fall flag set
	{
		fall_state = GPIO_PinInGet(GPIO_FALL_INT_PORT, GPIO_FALL_INT_PIN);

		LOG_INFO("State of fall in interrupt = %d", fall_state);

		interrupt_flags_set |= FALL_INT_MASK;
	}
	if(flag == 0x80) // Tap config button
	{
		tap_config_button = GPIO_PinInGet(PB1_PORT, PB1_PIN);

		LOG_INFO("Configure tap detection\n");

		interrupt_flags_set |= TAP_CONFIG_BUTTON;

	}
	if(flag == 0x08) // Tap flag set
	{
		tap_state = GPIO_PinInGet(GPIO_TAP_INT_PORT, GPIO_TAP_INT_PIN);

		LOG_INFO("State of tap gpio in interrupt = %d", tap_state);

		interrupt_flags_set |= TAP_INT_MASK;
	}

	GPIO_IntClear(flag);


	gecko_external_signal(interrupt_flags_set); // Set gecko external event
}
