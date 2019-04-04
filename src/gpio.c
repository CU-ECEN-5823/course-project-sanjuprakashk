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

void PB0_interrupt_enable()
{
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);

	GPIO_IntConfig(PB0_PORT, PB0_PIN, true, true, true); // Enable GPIO interrupt
}
void GPIO_EVEN_IRQHandler()
{
	int flag = GPIO_IntGet();

	pin_state = GPIO_PinInGet(PB0_PORT, PB0_PIN);

	if(!pin_state)
	{
		pin_pressed_flag = 1;
		LOG_INFO("State of pin pressed flag in interrupt = %d", pin_pressed_flag);
	}

	LOG_INFO("State of pin in interrupt = %d", pin_state);

	CORE_ATOMIC_IRQ_DISABLE();
	interrupt_flags_set |= BUTTON_INT_MASK;


	GPIO_IntClear(flag);


	gecko_external_signal(interrupt_flags_set); // Set gecko external event

}
