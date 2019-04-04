/*
 * File : cmu_config.c
 *
 *  Created on: Jan 30, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: an0004.1-efm32-cmu.pdf
 *
 */

#include "cmu_config.h"
#include "letimer.h"

extern const int lowest_sleep_mode;
/**
--------------------------------------------------------------------------------------------
cmu_Init
--------------------------------------------------------------------------------------------
*	This function works as an Init for the clock management unit
*
* 	@\param			void
*
* 	@\return		void
*
**/

void cmu_Init()
{
	if(lowest_sleep_mode < 3)
	{
		 CMU_OscillatorEnable(cmuOsc_LFXO , true, true); // Enable LFXO Oscillator for EM0, EM1, EM2
		 CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_LFXO ); // Select LFA from LFXO
		 CMU_ClockEnable(cmuClock_LFA, true ); // Enable LFA Clock
		 CMU_ClockEnable(cmuClock_LETIMER0, true); // Enable LETIMER0 Clock
	}
	else if(lowest_sleep_mode >= 3)
	{
		CMU_OscillatorEnable(cmuOsc_ULFRCO , true, true); // Enable ULFRCO Oscillator for EM3
		CMU_ClockSelectSet(cmuClock_LFA , cmuSelect_ULFRCO ); // Select LFA from ULFRCO
		CMU_ClockEnable(cmuClock_LFA, true ); // Enable LFA Clock
		CMU_ClockEnable(cmuClock_LETIMER0, true); // Enable LETIMER0 Clock
	}
}

