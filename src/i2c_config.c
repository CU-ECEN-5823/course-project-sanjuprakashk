/*
 * File : i2c_config.c
 *
 *  Created on: Apr 13, 2019
 *  Author: Sanju Prakash Kannioth
 *  Reference: https://www.silabs.com/community/blog.entry.html/2016/03/09/chapter_10_2_contro-l8qA,
 *  		   http://cache.freescale.com/files/sensors/doc/app_note/AN4070.pdf
 */
#include "i2c_config.h"


int timeout = 30000;

I2C_TransferReturn_TypeDef result;

/**
--------------------------------------------------------------------------------------------
i2c_init
--------------------------------------------------------------------------------------------
*	This function works as the initialization for the I2C
*
* 	@\param			void
*
* 	@\return		void
*
**/

int8_t i2c_init()
{
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
	I2CSPM_Init(&i2cInit); // Initialize I2C
//	I2C_Enable(I2C0, 0); // Disabling I2C0
	LOG_INFO("Init Complete\n");

	return 0;
}

/**
--------------------------------------------------------------------------------------------
i2c_transfer
--------------------------------------------------------------------------------------------
*	This function works as the transfer function for the I2C
*
* 	@\param			device_address, cmd_array, data_array, data_length, cmd_length, flags
*
* 	@\return		0 on success
*
**/
int8_t i2c_transfer(uint16_t device_address, uint8_t cmd_array[], uint8_t data_array[], uint16_t data_length, uint8_t cmd_length, uint8_t flags)
{

	I2C_TransferSeq_TypeDef i2cTransfer;

	i2cTransfer.addr          = device_address;
	i2cTransfer.flags         = flags;
	i2cTransfer.buf[0].data   = cmd_array;
	i2cTransfer.buf[0].len    = cmd_length;

	i2cTransfer.buf[1].data   = data_array;
	i2cTransfer.buf[1].len    = data_length;

	NVIC_EnableIRQ(I2C0_IRQn);

	result = I2C_TransferInit(I2C0, &i2cTransfer);
	while (!sch_event.tx_done && timeout != 0)
	{
		timeout--;
	}


	LOG_INFO("SLEEP MODE : %d\n",SLEEP_LowestEnergyModeGet());

	CORE_ATOMIC_IRQ_DISABLE();
	sch_event.tx_done = 0;
	CORE_ATOMIC_IRQ_ENABLE();

	NVIC_DisableIRQ(I2C0_IRQn);

	return 0;
}

/**
--------------------------------------------------------------------------------------------
i2c_read
--------------------------------------------------------------------------------------------
*	This function works as the read function for the I2C
*
* 	@\param			offset, data_len
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t i2c_read(uint8_t offset, uint8_t data_len)
{
	cmd_array[0] = offset;
	if(i2c_transfer(ACCEL_SENSOR, cmd_array, data_array, data_len, 1, I2C_FLAG_WRITE_READ) == 0)
	{
		LOG_DEBUG("I2C Read transfer success\n");
		return 0;
	}

	else
	{
		LOG_ERROR("Failed I2C Read\n");
		return -1;
	}


}

/**
--------------------------------------------------------------------------------------------
i2c_write
--------------------------------------------------------------------------------------------
*	This function works as the write function for the I2C
*
* 	@\param			offset, write_data
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t i2c_write(uint8_t offset, uint8_t write_data)
{
	cmd_array[0] = offset;
	data_array[0] = write_data;
	if(i2c_transfer(ACCEL_SENSOR, cmd_array, data_array, 1, 1, I2C_FLAG_WRITE_WRITE) == 0)
	{
			LOG_DEBUG("I2C Write transfer success\n");
			return 0;
	}

	else
	{
		LOG_ERROR("Failed I2C Write\n");
		return -1;
	}
}

/**
--------------------------------------------------------------------------------------------
accel_config_freefall
--------------------------------------------------------------------------------------------
*	This function works as the freefall configuration for MMA8452Q Accelerometer
*
* 	@\param			void
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t accel_config_freefall()
{
	//Set device in 50 Hz ODR, Standby
	if(i2c_write(0x2A, 0x20) != 0)
	{
	  LOG_ERROR("Failed in putting device in standby mode\n");
	  return -1;
	}

	if(i2c_read(0x2A, 1) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	  return -1;
	}

	// Setup configuration register
	if(i2c_write(0x15, 0xB8) != 0)
		return -1;

	// Setting threshold to < 0.4g
	if(i2c_write(0x17, 0x07) != 0)
		return -1;

	// Setup debounce count to 1
	if(i2c_write(0x18, 0x01) != 0)
		return -1;

	//Enable freefall interrupt
	if(i2c_read(0x2D, 1) != 0)
		return -1;

	int temp = data_array[0];
	temp = 0x04;

	//Enable freefall interrupt
	if(i2c_write(0x2D, temp) != 0)
		return -1;

	//Route freefall interrupt to INT2 Pin
	if(i2c_write(0x2E, 0x00) != 0)
		return -1;

	if(i2c_read(0x2A, 1) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	}

	temp = data_array[0];
	temp |= 0x01;

	LOG_INFO("CNTRL REG = %x", temp);

	// Put device in active mode
	if(i2c_write(0x2A, temp) != 0)
	{
	  LOG_ERROR("Failed in putting device in active mode\n");
	}


	return 0;
}


/**
--------------------------------------------------------------------------------------------
accel_config_tap
--------------------------------------------------------------------------------------------
*	This function works as the tap configuration for MMA8452Q Accelerometer
*
* 	@\param			void
*
* 	@\return		0 on success
* 					-1 on failure
*
**/
int8_t accel_config_tap()
{
	//400 Hz, Standby Mode
	if(i2c_write(0x2A, 0x08) != 0)
	{
	  LOG_ERROR("Failed in putting device in standby mode\n");
	  return -1;
	}

	if(i2c_read(0x2A, 1) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	  return -1;
	}


	if(i2c_write(0x21, 0x2A) != 0)
		return -1;

	// Set X Threshold
	if(i2c_write(0x23, 0x15) != 0)
		return -1;

	// Set Y Threshold
	if(i2c_write(0x24, 0x15) != 0)
		return -1;

	// Set Z Threshold
	if(i2c_write(0x25, 0x2A) != 0)
		return -1;

	// Set time limit
	if(i2c_write(0x26, 0x06) != 0)
		return -1;

	// Set latency time
	if(i2c_write(0x27, 0x50) != 0)
		return -1;


	// Set time window
	if(i2c_write(0x28, 0x78) != 0)
		return -1;

	if(i2c_read(0x2D, 1) != 0)
		return -1;

	int temp = data_array[0];
	temp = 0x08;

	// Setup interrupt
	if(i2c_write(0x2D, temp) != 0) // Set INT 1
		return -1;

	if(i2c_read(0x2E, 1) != 0)
		return -1;

	temp = data_array[0];
	temp = 0x08;

	// Route interrupt to INT1 Pin
	if(i2c_write(0x2E, temp) != 0)
		return -1;

	if(i2c_read(0x2A, 1) != 0)
	{
	  LOG_ERROR("Failed in Read\n");
	  return -1;
	}

	temp = data_array[0];
	temp |= 0x01;

	LOG_INFO("CNTRL REG = %x", temp);

	// Put device in active mode
	if(i2c_write(0x2A, temp) != 0)
	{
		LOG_ERROR("Failed in putting device in active mode\n");
		return -1;
	}

	return 0;
}

/**
--------------------------------------------------------------------------------------------
I2C0_IRQHandler
--------------------------------------------------------------------------------------------
*	This function works as the I2C0 IRQ Handler
*
* 	@\param			void
*
* 	@\return		void
*
**/
void I2C0_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();

	result = I2C_Transfer(I2C0);

	if(result == i2cTransferDone)
	{
		sch_event.tx_done = 1; // Set I2C0 transfer complete event
	}
	else if(result != i2cTransferInProgress)
	{
		sch_event.tx_err = 1; // Set I2C0 transfer error event
	}

	CORE_ATOMIC_IRQ_ENABLE();
}
