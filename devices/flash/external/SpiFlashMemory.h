///	@file       devices/flash/SpiFlashMemory.h
///	@class      SpiFlashMemory
///	@brief      An interface to use external SPI FLASH memory like a filesystem.
///
/// @note       This code is part of the `stm32-toolbox` project that provides easy-to-use building blocks to create
///             firmware for STM32 microcontrollers. _See https://github.com/TwoRedCells/stm32-toolbox/_
/// @copyright  See https://github.com/TwoRedCells/stm32-toolbox/blob/main/LICENSE


#ifndef INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORY_H_
#define INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORY_H_


#include <stdint.h>
#include <memory.h>
#include "comms/SPI.h"


/**
 * @brief	An interface to use external SPI FLASH memory.
 */
class SpiFlashMemory : public SPI
{
public:
	static constexpr uint8_t WriteEnable = 0x06;
	static constexpr uint8_t WriteDisable = 0x04;
	static constexpr uint8_t ReadStatusRegisterLow = 0x05;
	static constexpr uint8_t ReadStatusRegisterHigh = 0x35;
	static constexpr uint8_t WriteStatusRegister = 0x01;
	static constexpr uint8_t WriteEnableVolatileStatusRegister = 0x50;
	static constexpr uint8_t ReadDataBytes = 0x03;
	static constexpr uint8_t ReadDataBytesHighSpeed = 0x0b;
	static constexpr uint8_t DualOutputFastRead = 0x3b;
	static constexpr uint8_t QuadOutputFastRead = 0x6b;
	static constexpr uint8_t DualInputOutputFastRead = 0xbb;
	static constexpr uint8_t QuadInputOutputFastRead = 0xeb;
	static constexpr uint8_t QuadInputOutputWordFastRead = 0xe7;
	static constexpr uint8_t SetBurstWithWrap = 0x77;
	static constexpr uint8_t PageProgram = 0x02;
	static constexpr uint8_t QuadPageProgram = 0x32;
	static constexpr uint8_t SectorErase = 0x20;
	static constexpr uint8_t BlockErase32k = 0x52;
	static constexpr uint8_t BlockErase64k = 0xd8;
	static constexpr uint8_t ChipErase = 0x60;
	static constexpr uint8_t ChipEraseAlternate = 0xc7;
	static constexpr uint8_t DeepPowerDownEnable = 0xb9;
	static constexpr uint8_t DeepPowerDownDisable = 0xab;
	static constexpr uint8_t HighPerformanceMode = 0xa3;
	static constexpr uint8_t HighPerformanceModeReadDeviceId = 0xab;
	static constexpr uint8_t ContinuousReadModeReset = 0xff;
	static constexpr uint8_t ProgramEraseSuspend = 0x75;
	static constexpr uint8_t ProgramEraseResume = 0x7a;
	static constexpr uint8_t EraseSecurityRegisters = 0x44;
	static constexpr uint8_t ProgramSecurityRegisters = 0x42;
	static constexpr uint8_t ReadSecurityRegisters = 0x48;
	static constexpr uint8_t EnableReset = 0x66;
	static constexpr uint8_t Reset = 0x99;
	static constexpr uint8_t ReadSerialFlashDiscoverableParameter = 0x5a;
	static constexpr uint8_t ReadUniqueId = 0x4b;
	static constexpr uint8_t ReadManufacturerId = 0x90;
	static constexpr uint8_t ReadIdentification = 0x9f;

	typedef uint32_t error;
	static constexpr error ErrorNone = 0x00000000;
	static constexpr error ErrorMisalignedPage = 0x00000001;  /// The address provided does not align with a page boundary.
	static constexpr error ErrorLargerThanPage = 0x00000002;  /// An attempt was made to use program_page to program more than a page.
	static constexpr error ErrorVerifyFailed = 0x00000004;   /// The written data was read back and it does not match.

	static constexpr uint32_t PageSize = 0x100;
	static constexpr uint32_t SectorSize = 0x1000;

	typedef struct {
		uint8_t manufacturer;
		uint8_t type;
		uint8_t capacity;

		bool is_valid(void)
		{
			return capacity != 0 && !((manufacturer == type) == capacity);
		}
	} RDID;


	/**
	 * @brief	Initializes this instance with SPI port parameters.
	 * @param	hspi The SPI instance to use.
	 * @param	ss_pin The pin to use for SPI CS.
	 */
	SpiFlashMemory(SPI_HandleTypeDef* hspi, GPIO_TypeDef* cs_port, uint16_t cs_pin) : SPI(hspi, cs_port, cs_pin)
	{
	}


	/**
	 * @brief	Reads the identification structure.
	 * @returns	The identification structure.
	 */
	RDID read_identification(void)
	{
		while (!is_idle());  // Wait for any pending operations.
		RDID rdid;
		cs_select();
		write_byte(ReadIdentification);
		rdid.manufacturer = read_byte();
		rdid.type = read_byte();
		rdid.capacity = read_byte();
		cs_deselect();
		return rdid;
	}


	typedef struct {
		uint8_t manufacturer;
		uint8_t device;
	} REMS;


	/**
	 * @brief	Reads the REMS structure.
	 * @returns	The REMS structure.
	 */
	REMS read_manufacturer_id(void)
	{
		while (!is_idle());  // Wait for any pending operations.
		REMS rems;
		cs_select();
		write_byte(ReadManufacturerId);
		write_address(0x000000);
		rems.manufacturer = read_byte();
		rems.device = read_byte();
		cs_deselect();
		return rems;
	}


	typedef struct {
		uint8_t id[16];
	} UUID;


	/**
	 * @brief	Reads the chip's unique 128-bit ID.
	 *
	 */
	UUID read_unique_id()
	{
		while (!is_idle());  // Wait for any pending operations.
		UUID uuid;
		cs_select();
		write_byte(ReadUniqueId);
		write_address(0x000000);
		write_byte(0x00);
		read_bytes((uint8_t*) &uuid, 16);
		cs_deselect();
		return uuid;
	}


	typedef struct {
		uint8_t SRP1 : 1;  /// Status register protect 1
		uint8_t QE : 1;  /// Quad enable
		uint8_t LB : 1;  /// Lock
		uint8_t _reserved : 2;
		uint8_t HPF : 1;  /// High performance
		uint8_t CMP : 1;  /// CMP
		uint8_t SUS : 1;  /// Suspended

		uint8_t WIP : 1;  /// Write in progress
		uint8_t WEL : 1;  /// Write enable latch
		uint8_t BP0 : 1;  /// Block protect 0
		uint8_t BP1 : 1;  /// Block protect 1
		uint8_t BP2 : 1;  /// Block protect 2
		uint8_t BP3 : 1;  /// Block protect 3
		uint8_t BP4 : 1;  /// Block protect 4
		uint8_t SRP2 : 1;  /// Status register protect 2

	} STATUS_REGISTER;


	/**
	 * @brief	Reads the status register.
	 * @returns The status register.
	 */
	STATUS_REGISTER read_status_register(void)
	{
		cs_select();
		write_byte(ReadStatusRegisterLow);
		uint8_t low = read_byte();
		cs_deselect();
		cs_select();
		write_byte(ReadStatusRegisterHigh);
		uint8_t high = read_byte();
		cs_deselect();
		STATUS_REGISTER sr;
		uint8_t* srp = (uint8_t*) &sr;
		srp[0] = high;
		srp[1] = low;
		return sr;
	}


	/**
	 * @brief	Checks whether the chip is idle.
	 * @returns	True if idle; false if busy.
	 */
	bool is_idle(void)
	{
		STATUS_REGISTER sr = read_status_register();
		return !sr.WIP;
	}


	/**
	 * @brief	Enables writing (disables write-protection).
	 */
	void write_enable(void)
	{
		cs_select();
		write_byte(WriteEnable);
		cs_deselect();
	}


	/**
	 * @brief	Disables writing (enables write-protection).
	 */
	void write_disable(void)
	{
		cs_select();
		write_byte(WriteDisable);
		cs_deselect();
	}


	/**
	 * @brief	Erases the whole chip.
	 */
	void chip_erase(void)
	{
		while (!is_idle());  // Wait for any pending operations.
		write_enable();
		cs_select();
		write_byte(ChipErase);
		cs_deselect();
	}


	/**
	 * @brief	Erases a whole sector.
	 * @param	address Any valid address within the sector.
	 */
	void sector_erase(uint32_t address)
	{
		while (!is_idle());  // Wait for any pending operations.
		write_enable();
		cs_select();
		write_byte(SectorErase);
		write_address(address);
		cs_deselect();
	}


	/**
	 * @brief	Programs data.
	 * @param	address The address to start writing.
	 * @param	data Pointer to the data to write.
	 * @param	length The number of bytes to write.
	 * @returns	ErrorMisalignedPage if the length is greater than one page and address does not start on a page boundary.
	 * @note	Address does not need to start on a page boundary; however if it does not, any bytes written
	 *          past the page boundary will wrap to the beginning.
	 */
	error page_program(uint32_t address, void* data, uint32_t length=0x100)
	{
		if (length > 0x100)
			return ErrorLargerThanPage;

		while (!is_idle());  // Wait for any pending operations.
		write_enable();
		cs_select();
		write_byte(PageProgram);
		write_address(address);
		for (uint32_t i=0; i < length; i++)
			write_byte(((uint8_t*)data)[i]);
		cs_deselect();

		while (!is_idle());  // Wait for write.
		uint8_t verify[0x100];
		read(address, verify, length);
		error e = !memcmp(data, verify, length) ? ErrorNone : ErrorVerifyFailed;
		return e;
	}


	/**
	 * @brief	Programs data.
	 * @param	address The address to start writing.
	 * @param	data Pointer to the data to write.
	 * @param	length The number of bytes to write.
	 * @returns	ErrorMisalignedPage if the length is greater than one page and address does not start on a page boundary.
	 * @note	Address does not need to start on a page boundary; however if it does not, any bytes written
	 *          past the page boundary will wrap to the beginning. For this reason, writing more than 0x100
	 *          bytes other than at the start of a page is disallowed.
	 */
	error write(uint32_t address, void* data, uint32_t length)
	{
		if (address % 0x100 != 0 && length > 0x100)
			return ErrorMisalignedPage;

		for (uint32_t i=0; i < length; i += 0x100)
		{
			uint32_t r = (length-i) % 0x100;
			uint32_t l = length-i < 0x100 ? r : 0x100;
			error e = page_program(address+i, ((uint8_t*)data)+i, l);
			if (e != ErrorNone)
				return e;
		}
		return ErrorNone;
	}


	/**
	 * @brief	Reads data.
	 * @param	address The address to start reading.
	 * @param	data Pointer to the data buffer.
	 * @param	length The number of bytes to read.
	 * @returns	ErrorMisalignedPage if the length is greater than one page and address does not start on a page boundary.
	 */
	void read(uint32_t address, void* data, uint32_t length)
	{
		while (!is_idle());  // Wait for any pending operations.
		cs_select();
		write_byte(ReadDataBytes);
		write_address(address);
		for (uint32_t i=0; i < length; i++)
			((uint8_t*)data)[i] = read_byte();
		cs_deselect();
	}


	/**
	 * @brief	Reads data faster.
	 * @param	address The address to start reading.
	 * @param	data Pointer to the data buffer.
	 * @param	length The number of bytes to read.
	 * @returns	ErrorMisalignedPage if the length is greater than one page and address does not start on a page boundary.
	 */
	void read_fast(uint32_t address, void* data, uint32_t length)
	{
		while (!is_idle());  // Wait for any pending operations.
		cs_select();
		write_byte(ReadDataBytesHighSpeed);
		write_address(address);
		write_byte(0x00);
		for (uint32_t i=0; i < length; i++)
			((uint8_t*)data)[i] = read_byte();
		cs_deselect();
	}


private:
	/**
	 * @brief	Writes a 24-bit address.
	 * @param	address The address to write.
	 */
	void write_address(uint32_t address)
	{
		write_byte((address >> 16) & 0xff);
		write_byte((address >> 8) & 0xff);
		write_byte(address & 0xff);
	}
};

#endif /* INC_STM32_TOOLBOX_DEVICES_FLASH_SPIFLASHMEMORY_H_ */
