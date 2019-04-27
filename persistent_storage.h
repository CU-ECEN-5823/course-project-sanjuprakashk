/*
 * File : persistent_storage.h
 *
 *  Created on: Apr 26, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/05/02/how_to_save_floatva-Udm8
 */

#ifndef PERSISTENT_STORAGE_H
#define PERSISTENT_STORAGE_H

/*
 *  INCLUDES
 */
#include <native_gecko.h>

/*
 * MACROS
 */
#define PS_KEY_FALL_CONFIGURED 0x4000 // Persistent storage key value for fall
#define PS_KEY_TAP_CONFIGURED  0x4001 // Persistent storage key value for tap
#define PS_KEY_BUZZER_STATE    0x4002 // // Persistent storage key value for buzzer state


/**
--------------------------------------------------------------------------------------------
ps_save
--------------------------------------------------------------------------------------------
*	This function works as the persistent storage save function
*
* 	@\param			key, *pValue, size
*
* 	@\return		gecko_msg_flash_ps_save_rsp_t result
*
**/
uint16_t ps_save(uint16_t key, void *pValue, uint8_t size);

/**
--------------------------------------------------------------------------------------------
ps_load
--------------------------------------------------------------------------------------------
*	This function works as the persistent storage load function
*
* 	@\param			key, *pValue, size
*
* 	@\return		gecko_msg_flash_ps_save_rsp_t result
*
**/
uint16_t ps_load(uint16_t key, void *pValue, uint8_t size);

#endif
