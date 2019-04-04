/*
 * File : cmu_config.h
 *
 *  Created on: Jan 30, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: an0004.1-efm32-cmu.pdf
 *
 */
#ifndef CMU_CONFIG_H_
#define CMU_CONFIG_H_
#include <em_cmu.h>
#include "letimer.h"


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
void cmu_Init();
#endif
