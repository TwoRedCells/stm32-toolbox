///	@file       devices/flash/SPI.h
///	@class      SPI
///	@brief      Abstracts SPI hardware.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_SPI_H_
#define INC_STM32_TOOLBOX_DEVICES_SPI_H_


/**
 *	@brief Abstracts SPI hardware.
 */
class SPI
{
public:
	/**
	 * @brief	Initializes this instance with SPI port parameters.
	 * @param	hspi The SPI instance to use.
	 * @param	ss_pin The pin to use for SPI CS.
	 */
	void init(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin)
	{
		this->hspi = hspi;
		this->cs_port = cs_port;
		this->cs_pin = cs_pin;
	}

	void cs_select(void)
	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
	}


	void cs_deselect(void)
	{
		HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);
	}

	/**
	 * @brief	Writes a byte to the port.
	 * @param	data The data to write.
	 */
	void write_byte(uint8_t data)
	{
		write_bytes(&data, 1);
	}


	/**
	 * @brief	Writes a word to the port.
	 * @param	data The word to write.
	 */
	void write_word(uint16_t data)
	{
		write_byte((uint8_t)((data >> 8) & 0xFF));
		write_byte((uint8_t)(data & 0xFF));
	}


	/**
	 * @brief	Writes data to the port.
	 * @param 	data Pointer to the data to write.
	 * @param	len Number of bytes to write.
	 */
	void write_bytes(uint8_t *data, uint16_t len)
	{
		for (int i=0; i < len; i++)
			last_error = HAL_SPI_Transmit(hspi, &data[i], 1, 100);
	}


	/**
	 * @brief	Reads a byte from the port.
	 * @returns	The data.
	 */
	uint8_t read_byte(void)
	{
		uint8_t data;
		read_bytes(&data, 1);
		return data;
	}


	/**
	 * @brief	Reads a word from the port.
	 * @returns	The data.
	 */
	uint16_t read_word(void)
	{
		uint16_t data;
		data = read_byte() << 8;
		data |= read_byte();
		return data;
	}


	/**
	 * @brief	Reads bytes from the port.
	 * @param	data A pointer to the location to store the data.
	 * @param	len The number of bytes to read.
	 * @returns	The data.
	 */
	void read_bytes(uint8_t *data, uint16_t len)
	{
		for(int i = 0; i < len; i++)
			last_error = HAL_SPI_Receive(hspi, &data[i], 1, 100);
	}


	/**
	 * @brief 	Gets the last error.
	 * @returns The last error;
	 */
	HAL_StatusTypeDef get_last_error(void)
	{
		return last_error;
	}


private:
	SPI_HandleTypeDef* hspi;
	uint16_t cs_pin;
	GPIO_TypeDef* cs_port;
	HAL_StatusTypeDef last_error;
};

#endif /* INC_STM32_TOOLBOX_DEVICES_SPI_H_ */
