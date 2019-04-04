/*
 * File : letimer.c
 *
 *  Created on: Jan 30, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/documents/public/example-code/an0026-efm32-letimer.zip,
 *  		   https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/08/17/using_low_energytim-9RYB
 *
 */
#include "letimer.h"

extern const int lowest_sleep_mode;

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

void LETIMER0_init()
{
	/* Set configurations for LETIMER 0 */

	const LETIMER_Init_TypeDef letimer_Init = // Taken from silicon labs AN0026 example
	{
	  .enable         = true,                   /* Start counting when init completed. */
	  .debugRun       = false,                  /* Counter shall not keep running during debug halt. */
	  .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
	  .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
	  .out0Pol        = 0,                      /* Idle value for output 0. */
	  .out1Pol        = 0,                      /* Idle value for output 1. */
	  .ufoa0          = letimerUFOANone,        /* UF output 0 action */
	  .ufoa1          = letimerUFOANone,        /* UF output 1 action*/
	  .repMode        = letimerRepeatFree       /* Count until stopped */
	};

	LETIMER_Init(LETIMER0, &letimer_Init); // Initialize the LETIMER0
	LETIMER_IntEnable(LETIMER0, LETIMER_IEN_COMP0);// | LETIMER_IEN_COMP1); // Enable COMP0 and COMP1 interrupts

	NVIC_EnableIRQ(LETIMER0_IRQn);
	prescaler_calc();
	LETIMER_Enable(LETIMER0, true); // Enable LETIMER0
	timer_count = 0;
	disp_count = 0;

}

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
void prescaler_calc()
{
	uint32_t oneSecCalc,prescaleDiv = 1;
	divCalc = 1;
	if(lowest_sleep_mode < 3) // Logic for EM0, EM1, EM2 mode
	{
		if(TIME_PERIOD >= 2) // Check if time period is greater that 2 seconds
		{
			oneSecCalc = TIME_PERIOD * HIGH_FREQ_OSC; //
			uint32_t scaler = oneSecCalc/65535;
			while(prescaleDiv<=scaler)
			{
				divCalc *= 2;
				prescaleDiv++;
			}
			scaledFreq = HIGH_FREQ_OSC/divCalc;

			CMU_ClockDivSet(cmuClock_LETIMER0,divCalc); // Set the prescaler to calculated value
			newOneSecCalc = TIME_PERIOD * scaledFreq;

			LETIMER_CompareSet(LETIMER0,0,newOneSecCalc); // Set the COMP0 interrupt count


		}
		else
		{
			newOneSecCalc = TIME_PERIOD * HIGH_FREQ_OSC;
			scaledFreq = HIGH_FREQ_OSC;

			LETIMER_CompareSet(LETIMER0,0,newOneSecCalc); // Set the COMP0 interrupt count

		}
	}
	else // Logic for EM3 mode
	{
		if(TIME_PERIOD >= 2)
		{
			oneSecCalc = TIME_PERIOD * LOW_FREQ_OSC;
			uint32_t scaler = oneSecCalc/65535;
			while(prescaleDiv<=scaler)
			{
				divCalc *= 2;
				prescaleDiv++;
			}
			scaledFreq = LOW_FREQ_OSC/divCalc;

			CMU_ClockDivSet(cmuClock_LETIMER0,divCalc);
			newOneSecCalc = TIME_PERIOD * scaledFreq;

			LETIMER_CompareSet(LETIMER0,0,newOneSecCalc); // Set the COMP0 interrupt count
		}
		else
		{
			newOneSecCalc = TIME_PERIOD * LOW_FREQ_OSC;
			scaledFreq = LOW_FREQ_OSC;

			LETIMER_CompareSet(LETIMER0,0,newOneSecCalc); // Set the COMP0 interrupt count
		}
	}
}

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

void LETIMER0_IRQHandler()
{
	uint32_t flags;
	CORE_ATOMIC_IRQ_DISABLE();


	flags = LETIMER_IntGet(LETIMER0); // Gets the current interrupt status flags

	if(flags & LETIMER_IF_COMP0) // Compares the flag with COMP0 value
	{
		disp_count+=1;
		LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP0); //Clears the COMP0 interrupt flag
		interrupt_flags_set |= DISP_INT_MASK; // Setting Display interrupt bit mask
		timer_count+=1;
	}

	CORE_ATOMIC_IRQ_ENABLE();

	gecko_external_signal(interrupt_flags_set); // Set gecko external event

}
