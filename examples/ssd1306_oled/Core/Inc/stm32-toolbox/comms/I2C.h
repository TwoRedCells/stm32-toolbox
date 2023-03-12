/**
 * \file       comms/I2C.h
 * \class      i2c
 * \brief      Encapsulates i²c communications.
 */

#ifndef INC_COMMS_I2C_H_
#define INC_COMMS_I2C_H_


class i2c
{
public:

    /**
     * Performs initialization tasks for the bus.
     * @param handle Handle to the hardware resource.
     * @return True on success; otherwise false.
     */
	bool setup(I2C_HandleTypeDef *handle)
	{
		this->handle = handle;
		handle->Instance = I2C2;
		handle->Init.ClockSpeed = 400000;
		handle->Init.DutyCycle = I2C_DUTYCYCLE_2;
		handle->Init.OwnAddress1 = 0;
		handle->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		handle->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		handle->Init.OwnAddress2 = 0;
		handle->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		handle->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		if (HAL_I2C_Init(handle) != HAL_OK)
			return false;
		return true;
	}

protected:
	I2C_HandleTypeDef *handle;
};


#endif /* INC_COMMS_I2C_H_ */
