#include "i2c_config.h"


int timeout = 300000;
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

void i2c_init()
{
	I2CSPM_Init_TypeDef i2cInit = I2CSPM_INIT_DEFAULT;
	I2CSPM_Init(&i2cInit); // Initialize I2C
//	I2C_Enable(I2C0, 0); // Disabling I2C0
	LOG_INFO("Init Complete");
}


void i2c_transfer(uint16_t device_address, uint8_t cmd_array[], uint8_t data_array[], uint16_t data_length, uint8_t cmd_length, uint8_t flags)
{
	I2C_TransferSeq_TypeDef i2cTransfer;

	I2C_TransferReturn_TypeDef result;
	i2cTransfer.addr          = device_address;
	i2cTransfer.flags         = flags;
	i2cTransfer.buf[0].data   = cmd_array;
	i2cTransfer.buf[0].len    = cmd_length;

	i2cTransfer.buf[1].data   = data_array;
	i2cTransfer.buf[1].len    = data_length;

	result = I2C_TransferInit(I2C0, &i2cTransfer);

	while (result != i2cTransferDone && timeout--)
	{
		if(result != i2cTransferInProgress)
		{
			LOG_INFO("result = %d\n", result);
		}
		result = I2C_Transfer(I2C0);
	}
	LOG_INFO("result = %d\n", result);
}

uint8_t i2c_read(uint8_t offset, uint8_t data_len)
{
	cmd_array[0] = offset;
	i2c_transfer(ACCEL_SENSOR, cmd_array, data_array, data_len, 1, I2C_FLAG_WRITE_READ);

	return data_array[0];
}

void i2c_write(uint8_t offset, uint8_t write_data)
{
	cmd_array[0] = offset;
	data_array[0] = write_data;
	i2c_transfer(ACCEL_SENSOR, cmd_array, data_array, 1, 1, I2C_FLAG_WRITE_WRITE);

}

void accel_config()
{
	i2c_write(0x2A, 0x20); // Standby

	i2c_write(0x15, 0xB8);

	i2c_write(0x17, 0x03);

	i2c_write(0x18, 0x02);

	i2c_write(0x2D, 0x04);

	i2c_write(0x2E, 0x00);

	i2c_read(0x2A, 1);

	int temp = data_array[0];
	temp |= 0x01;

	LOG_INFO("CNTRL REG = %x", temp);

	i2c_write(0x2A, temp);
}
