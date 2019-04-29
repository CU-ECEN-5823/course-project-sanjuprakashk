/*
 * File : persistent_storage.c
 *
 *  Created on: Apr 26, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/community/wireless/bluetooth/knowledge-base.entry.html/2017/05/02/how_to_save_floatva-Udm8
 */


#include "persistent_storage.h"

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
uint16_t ps_save(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_save_rsp_t *pResp;

	pResp = gecko_cmd_flash_ps_save(key, size, pValue);

	return(pResp->result);
}


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
uint16_t ps_load(uint16_t key, void *pValue, uint8_t size)
{
	struct gecko_msg_flash_ps_load_rsp_t *pResp;

	pResp = gecko_cmd_flash_ps_load(key);

	if(pResp->result == 0)
	{
		memcpy(pValue, pResp->value.data, pResp->value.len);

		// sanity check: length of data stored in PS key must match the expected value
		if(size != pResp->value.len)
		{
			return(bg_err_unspecified);
		}
	}

	return(pResp->result);
}
