#ifndef ONE_WIRE_UART_LIB_h
#define ONE_WIRE_UART_LIB_h

/**
 ******************************************************************************
 * @file           : OneWire.h
 * @brief          : HAL library to work with OneWire bus
 ******************************************************************************
 * @attention
 * Copyright 2021 Konstantin Toporov
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom 
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL 
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR 
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
 * OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ********************* Description *************************
 * This library is based on the labrary taken from 
 * https://stm32f4-discovery.net/2015/07/hal-library-05-onewire-for-stm32fxxx/
 * Unlike that library this one doesn't use delays.
 * It use system UART to communicate with OneWire device(s).
 * 
 * UART must be set before calling this function
 * Settings of UART:
 * Asynchronous mode, global interrupt enabled.
 * Baud Rate - any, it will be reset in the reset function.
 * 8-n-1
 * Connection of RX & TX should be done as specified here 
 * https://www.maximintegrated.com/en/design/technical-documents/tutorials/2/214.html
 * or use a diod to connect RX and TX to 1Wire bus
 * +3.3V----+
 *          |
 *          П 4.7k Resistor
 *          U
 *          |
 * RX ------+-------- to 1Wire bus
 *          |
 * TX ---K|-+ Diod Cathode to TX pin
 * 
 *  -K|- any fast switching diod for exmaple FR103
 * 
 */


#define WIRE_1 0xFF
#define WIRE_0 0x00

#define OW_RESET_SPEED 9600
#define OW_WORK_SPEED 115200

#define OW_TIMEOUT 5 //1 ms is enouph

/* OneWire commands */
#define OW_CMD_RSCRATCHPAD			0xBE
#define OW_CMD_WSCRATCHPAD			0x4E
#define OW_CMD_CPYSCRATCHPAD		0x48
#define OW_CMD_RECEEPROM			0xB8
#define OW_CMD_RPWRSUPPLY			0xB4
#define OW_CMD_SEARCHROM			0xF0
#define OW_CMD_READROM				0x33
#define OW_CMD_MATCHROM			    0x55
#define OW_CMD_SKIPROM				0xCC

class OneWire
{
public:
	OneWire(UART_HandleTypeDef *huart)
	{
		this->huart = huart;
		status = HAL_OK;
	}


	/**
	 * If set baud rate with Deinit and Init there will be an unneeded byte 0xF0
	 * on the bus
	 */
	void set_baud_rate(UART_HandleTypeDef *huart, uint32_t bdr)
	{
//		uint32_t pclk;
//		if(huart->Instance == USART1)
//			pclk = HAL_RCC_GetPCLK2Freq();
//		else
//			pclk = HAL_RCC_GetPCLK1Freq();
//		__HAL_UART_DISABLE(huart); //Not sure it is needed
//		huart->Instance->BRR = UART_BRR_SAMPLING16(pclk, bdr);
//		__HAL_UART_ENABLE(huart); //Not sure it is needed
	}


	uint8_t reset(void)
	{
		if (status != HAL_OK)
			reset_uart();

		uint8_t reset = 0xF0;
		uint8_t resetBack = 0;

		set_baud_rate(huart, OW_RESET_SPEED);
		HAL_UART_Transmit_IT(huart, &reset, 1);
		status = HAL_UART_Receive(huart, &resetBack, 1, OW_TIMEOUT);
		set_baud_rate(huart, OW_WORK_SPEED);
		return reset != resetBack;
	}


	void send_bit(uint8_t b)
	{
		uint8_t r, s;
		s = b ? WIRE_1 : WIRE_0;
		HAL_UART_Transmit_IT(huart, &s, 1);
		status = HAL_UART_Receive(huart, &r, 1, OW_TIMEOUT);
	}


	uint8_t receive_bit(void)
	{
		uint8_t s = 0xFF, r;
		HAL_UART_Transmit_IT(huart, &s, 1);
		status = HAL_UART_Receive(huart, &r, 1, OW_TIMEOUT);
		return r == 0xFF;
	}


	void send_byte(uint8_t b)
	{
		uint8_t sendByte[8];
		byte_to_bits(b, sendByte); //0b01101001 => 0x00 0xFF 0xFF 0x00 0xFF 0x00 0x00 0xFF

		for(uint8_t i=0;i<8;i++)
			send_bit(sendByte[i]);
		/*
		On a high loaded system there will be desynchronization of transmit and receive
		buffer. It will lead to timeout errors.
		I'm not sure if deinit and init will clear that error.
		Below the example that creates such error.
		 */
		//HAL_UART_Transmit_IT(huart, sendByte, 8);
		//status = HAL_UART_Receive(huart, recvByte, 8, OW_TIMEOUT);
	}


	void send_bytes(uint8_t *bytes, uint8_t len)
	{
		for(uint8_t i=0; i<len; i++)
			send_byte(bytes[i]);
	}


	uint8_t receive_byte(void)
	{
		uint8_t sendByte[8];
		uint8_t recvByte[8];
		byte_to_bits(0xFF, sendByte);

		for (uint8_t i=0; i<8; i++)
			recvByte[i] = receive_bit();

		return bits_to_byte(recvByte);
	}


	void receive_bytes(uint8_t *bytes, uint8_t len)
	{
		for(uint8_t i=0; i<len; i++)
			bytes[i] = receive_byte();
	}


	uint8_t crc8(uint8_t* addr, uint8_t len)
	{
		uint8_t crc = 0, inbyte, i, mix;

		while (len--)
		{
			inbyte = *addr++;
			for (i = 8; i; i--)
			{
				mix = (crc ^ inbyte) & 0x01;
				crc >>= 1;
				if (mix)
					crc ^= 0x8C;
				inbyte >>= 1;
			}
		}

		return crc;
	}


	void reset_search(void)
	{
		LastDiscrepancy = 0;
		LastDeviceFlag = 0;
		LastFamilyDiscrepancy = 0;
	}


	uint8_t first(void)
	{
		reset_search();
		return search(OW_CMD_SEARCHROM);
	}


	uint8_t search(uint8_t command)
	{
		uint8_t id_bit_number;
		uint8_t last_zero, rom_byte_number, search_result;
		uint8_t id_bit, cmp_id_bit;
		uint8_t rom_byte_mask, search_direction;

		/* Initialize for search */
		id_bit_number = 1;
		last_zero = 0;
		rom_byte_number = 0;
		rom_byte_mask = 1;
		search_result = 0;

		/* Check if any devices */
		if (!LastDeviceFlag)
		{
			/* 1-Wire reset */
			if (!reset())
			{
				/* Reset the search */
				reset_search();
				return 0; //Reset failed
			}

			/* Issue the search command */
			send_byte(command);

			/* Loop to do the search */
			do
			{
				/* Read a bit and its complement */
				id_bit = receive_bit(); //0
				cmp_id_bit = receive_bit(); //1

				/* Check for no devices on 1-wire */
				if ((id_bit == 1) && (cmp_id_bit == 1))
				{
					break;
				}
				else
				{
					/* All devices coupled have 0 or 1 */
					if (id_bit != cmp_id_bit)
					{
						/* Bit write value for search */
						search_direction = id_bit; //1
					}
					else
					{
						/* If this discrepancy is before the Last Discrepancy on a previous next then pick the same as last time */
						if (id_bit_number < LastDiscrepancy)
						{
							search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
						}
						else
						{
							/* If equal to last pick 1, if not then pick 0 */
							search_direction = (id_bit_number == LastDiscrepancy);
						}

						/* If 0 was picked then record its position in LastZero */
						if (search_direction == 0)
						{
							last_zero = id_bit_number;

							/* Check for Last discrepancy in family */
							if (last_zero < 9)
								LastFamilyDiscrepancy = last_zero;
						}
					}

					/* Set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask */
					if (search_direction == 1)
						ROM_NO[rom_byte_number] |= rom_byte_mask; // |= 1
					else
						ROM_NO[rom_byte_number] &= ~rom_byte_mask;

					/* Serial number search direction write bit */
					send_bit(search_direction);  //1

					/* Increment the byte counter id_bit_number and shift the mask rom_byte_mask */
					id_bit_number++; // 1 -> 2
					rom_byte_mask <<= 1; // 0b10

					/* If the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask */
					if (rom_byte_mask == 0)
					{
						rom_byte_number++;
						rom_byte_mask = 1;
					}
				}
				/* Loop until through all ROM bytes 0-7 */
			}
			while (rom_byte_number < 8);

			/* If the search was successful then */
			if (!(id_bit_number < 65))
			{
				/* Search successful so set LastDiscrepancy, LastDeviceFlag, search_result */
				LastDiscrepancy = last_zero;

				/* Check for last device */
				if (LastDiscrepancy == 0)
					LastDeviceFlag = 1;

				search_result = 1;
			}
		}

		/* If no device found then reset counters so next 'search' will be like a first */
		if (!search_result || !ROM_NO[0])
		{
			reset_search();
			search_result = 0;
		}
		return search_result;
	}


	uint8_t next(void)
	{
		/* Leave the search state alone */
		return search(OW_CMD_SEARCHROM);
	}


	uint8_t get_rom(uint8_t index)
	{
		return ROM_NO[index];
	}


	void get_full_rom(uint8_t *firstIndex)
	{
		uint8_t i;
		for (i = 0; i < 8; i++)
			*(firstIndex + i) = ROM_NO[i];
	}

	void select(uint8_t* addr)
	{
		uint8_t i;
		send_byte(OW_CMD_MATCHROM);

		for (i = 0; i < 8; i++)
			send_byte(*(addr + i));
	}

	void select_with_pointer(uint8_t* ROM)
	{
		uint8_t i;
		send_byte(OW_CMD_MATCHROM);

		for (i = 0; i < 8; i++)
			send_byte(*(ROM + i));
	}

private:
	/**
	 * internal function to reset uart when an error happens
	 * It seems this function is unneeded when 8 bits are sent separatly
	 */
	void reset_uart(void)
	{
//		HAL_UART_DeInit(huart);
//
//		huart->Instance = USART6;
//		huart->Init.BaudRate = 115200;
//		huart->Init.WordLength = UART_WORDLENGTH_8B;
//		huart->Init.StopBits = UART_STOPBITS_1;
//		huart->Init.Parity = UART_PARITY_NONE;
//		huart->Init.Mode = UART_MODE_TX_RX;
//		huart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
//		huart->Init.OverSampling = UART_OVERSAMPLING_16;
//		HAL_UART_Init(huart);
//		status = HAL_OK;
	}


	uint8_t bits_to_byte(uint8_t *bits)
	{
		uint8_t target_byte, i;
		target_byte = 0;
		for (i = 0; i < 8; i++)
		{
			target_byte = target_byte >> 1;
			if (*bits != WIRE_0)
				target_byte |= 0b10000000;
			bits++;
		}
		return target_byte;
	}


	/// Convert one byte to array of 8 bytes
	uint8_t *byte_to_bits(uint8_t ow_byte, uint8_t *bits)
	{
		uint8_t i;
		for (i = 0; i < 8; i++)
		{
			if (ow_byte & 0x01)
				*bits = WIRE_1;
			else
				*bits = WIRE_0;
			bits++;
			ow_byte = ow_byte >> 1;
		}
		return bits;
	}

	uint8_t LastDiscrepancy = 0;       /*!< Search private */
	uint8_t LastFamilyDiscrepancy = 0; /*!< Search private */
	uint8_t LastDeviceFlag = 0;        /*!< Search private */
	uint8_t ROM_NO[8]  = {0};             /*!< 8-bytes address of last search device */
	UART_HandleTypeDef *huart;
	HAL_StatusTypeDef status = HAL_OK;
};

#endif /* of ONE_WIRE_UART_LIB_h */
