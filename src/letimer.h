/*
 * File : letimer.h
 *
 *  Created on: Jan 30, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/documents/public/example-code/an0026-efm32-letimer.zip
 *
 */

#ifndef LETIMER_H_
#define LETIMER_H_

#include "em_letimer.h"
#include "src/gpio.h"
#include "log.h"
#include "em_core.h"
#include "em_cmu.h"
#include "native_gecko.h"
//#include "main.h"


#define COMP0_INT_MASK (1UL << 0)
#define COMP1_INT_MASK (1UL << 1)
#define DISP_INT_MASK (1UL << 4)


#define TIME_PERIOD (1) // Defines the total time period
#define SCHEDULER_EVENT (1) // Defines the scheduler event set value
#define HIGH_FREQ_OSC (32768)
#define LOW_FREQ_OSC (1000)

uint32_t scaledFreq;
uint32_t newOneSecCalc;
uint32_t divCalc;
uint32_t timer_count;
uint32_t disp_count;
extern uint32_t interrupt_flags_set;
/**
--------------------------------------------------------------------------------------------
LETIMER0_init
--------------------------------------------------------------------------------------------
*	This function works as an Init for letimer0
*
* 	@\param			void
*
* 	@\return		void
*
**/
void LETIMER0_init();

/**
--------------------------------------------------------------------------------------------
LETIMER0_IRQHandler
--------------------------------------------------------------------------------------------
*	This function works as an interrupt handler for letimer0
*
* 	@\param			void
*
* 	@\return		void
*
**/
void LETIMER0_IRQHandler();

/**
--------------------------------------------------------------------------------------------
prescaler_calc
--------------------------------------------------------------------------------------------
*	This function calculates the prescalar value
*
* 	@\param			void
*
* 	@\return		void
*
**/
void prescaler_calc();
#endif
