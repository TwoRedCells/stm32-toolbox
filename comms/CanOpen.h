/**
 * \file       comms/CanOpen.h
 * \class      CanOpen
 * \brief      Encapsulates CANopen communications.
 * \notes	Configure the CAN port using STM32CubeIDE normally. Create an interrupt, and put a call to on_message in the interrupt.
 */

#ifndef INC_COMMS_CANOPEN_H_
#define INC_COMMS_CANOPEN_H_

#include "CanBus.h"
#include "utility/FastDelegate.h"
#include "toolbox.h"

class CanOpen : public CanBus
{
public:
	static constexpr uint8_t NMT_Operational = 0x01;
	static constexpr uint8_t NMT_Stopped = 0x02;
	static constexpr uint8_t NMT_Preoperational = 0x80;
	static constexpr uint8_t NMT_ResetNode = 0x81;
	static constexpr uint8_t NMT_ResetCommunications = 0x82;

	// Indices 0x1000 to 0x1fff are defined by CIA 301.
	static constexpr uint16_t Index_DeviceType = 0x1000;  // 32-ro
	static constexpr uint16_t Index_ErrorRegister = 0x1001;  // 8-ro
	static constexpr uint16_t Index_CobIdSync = 0x1005;  // 32-rw
	static constexpr uint16_t Index_HardwareVersion = 0x1009;  // 16-ro
	static constexpr uint16_t Index_SoftwareVersion = 0x100a;  // 16-ro
	static constexpr uint16_t Index_CobIdEmcy = 0x1014;  // 32-ro
	static constexpr uint16_t Index_HeartbeatInterval = 0x1017;  // 16-rw

	static constexpr uint16_t Index_Manufacturer = 0x1018;  // 8-ro
	static constexpr uint16_t Subindex_VendorId = 0x01;  // 32-ro
	static constexpr uint16_t Subindex_ProductCode = 0x02;  // 32-ro

	static constexpr uint16_t Index_Sdo = 0x1200;  // 8-ro
	static constexpr uint16_t Subindex_CobIdClientToServer = 0x01;  // 32-ro
	static constexpr uint16_t Subindex_CobIdServerToClient = 0x02;  // 32-ro

	static constexpr uint16_t Index_Rpdo0Communications = 0x1400;
	static constexpr uint16_t Index_Rpdo1Communications = 0x1401;
	static constexpr uint16_t Index_Rpdo2Communications = 0x1402;
	static constexpr uint16_t Index_Rpdo3Communications = 0x1403;
	static constexpr uint16_t Subindex_NumberOfEntries = 0x00;  // 8-ro
	static constexpr uint16_t Subindex_Rpdo0CobId = 0x01;  // 32-ro
	static constexpr uint16_t Subindex_TransmissionTime = 0x02;  // 8-rw
	static constexpr uint16_t Subindex_InhibitTime = 0x03;  // 16-rw
	static constexpr uint16_t Subindex_CompatibilityEntry = 0x04;  // 8-rw
	static constexpr uint16_t Subindex_EventTimer = 0x05;  // 16-rw

	static constexpr uint16_t Index_Rpdo0Mapping = 0x1600;  // Use same subindexes as above.
	static constexpr uint16_t Index_Rpdo1Mapping = 0x1601;
	static constexpr uint16_t Index_Rpdo2Mapping = 0x1602;
	static constexpr uint16_t Index_Rpdo3Mapping = 0x1603;

	static constexpr uint16_t Index_Tpdo0Communications = 0x1800;  // Use same subindexes as above.
	static constexpr uint16_t Index_Tpdo1Communications = 0x1801;
	static constexpr uint16_t Index_Tpdo2Communications = 0x1802;
	static constexpr uint16_t Index_Tpdo3Communications = 0x1803;

	static constexpr uint16_t Index_Tpdo0Mapping = 0x1a00;  // Use same subindexes as above.
	static constexpr uint16_t Index_Tpdo1Mapping = 0x1a01;
	static constexpr uint16_t Index_Tpdo2Mapping = 0x1a02;
	static constexpr uint16_t Index_Tpdo3Mapping = 0x1a03;

	// Indices 0x6000 to 0x7fff are defined by CIA 402.
	static constexpr uint16_t Index_LastFaultCode = 0x603f;
	static constexpr uint16_t Index_ControlWord = 0x6040;
	static constexpr uint16_t Index_StatusWord = 0x6041;
	static constexpr uint16_t Index_QuickStop = 0x605a;
	static constexpr uint16_t Index_CloseOperation = 0x605b;
	static constexpr uint16_t Index_DisableOperation = 0x605c;
	static constexpr uint16_t Index_HaltControl = 0x605d;
	static constexpr uint16_t Index_OperatingMode = 0x6060;
	static constexpr uint16_t Index_OperatingModeStatus = 0x6061;
	static constexpr uint16_t Index_ActualPosition = 0x6064;
	static constexpr uint16_t Index_ActualSpeed = 0x606c;
	static constexpr uint16_t Index_TargetTorque = 0x6071;
	static constexpr uint16_t Index_RealtimeTargetTorque = 0x6074;
	static constexpr uint16_t Index_ActualTorque = 0x6077;
	static constexpr uint16_t Index_TargetPosition = 0x607a;
	static constexpr uint16_t Index_MaximumSpeed = 0x6081;
	static constexpr uint16_t Index_StartStopSpeedInPositionMode = 0x6082;
	static constexpr uint16_t Index_AccelerationTime = 0x6083;
	static constexpr uint16_t Index_DecelerationTime= 0x6084;
	static constexpr uint16_t Index_EmergencyStopDecelerationTime = 0x6085;
	static constexpr uint16_t Index_TorqueSlope = 0x6087;
	static constexpr uint16_t Index_TargetSpeed = 0x60ff;

	enum roles { Master, Slave };

	class ICanOpenCallback
	{
	public:
		/**
		 * Called when an sdo message is received.
		 * @param address The address ID of the receiver.
		 * @param index The SDO index.
		 * @param subindex The SDO subindex.
		 * @param data The data (1-4 bytes).
		 */
		virtual void on_sdo(uint16_t node, uint16_t index, uint8_t subindex, uint8_t* data)
		{
		}


		/**
		 * Called when an pdo message is received.
		 * @param address The address ID of the receiver.
		 * @param index The PDO index.
		 * @param subindex The PDO subindex.
		 * @param data The data (1-8 bytes).
		 */
		virtual void on_pdo(uint16_t cob, uint8_t* data)
		{
		}


		/**
		 * Called when an heartbeat message is received.
		 * @param node The node ID of the recipient.
		 */
		virtual void on_heartbeat(uint8_t node)
		{
		}
		

		/**
		 * Called when an nmt message is received.
		 * @param data The data (1-8 bytes).
		 */
		virtual void on_nmt(uint8_t data)
		{
		}


		/**
		 * Called when an unrecognized message is received.
		 */
		virtual void on_other_message(uint16_t cob, uint8_t* data)
		{
		}
	};


	/**
	 * Constructs a CanOpen object.
	 * @param hcan Pointer to CAN device handle.
	 * @param roles The role to assume.
	 * @param allow_tpdo5 If true, TPDO5 (and higher) can be expected at 0x190 etc. at the cost of having fewer possible nodes.
	 */
	CanOpen(CAN_HandleTypeDef *hcan, roles role=Master, bool allow_tpdo5=false) : CanBus(hcan)
	{
		this->role = role;
		this->allow_tpdo5 = allow_tpdo5;
	}


	/**
	 * Sends the CANopen SYNC message.
	 */
	uint32_t sync(void)
	{
		uint8_t data[] = { 0xff, 0xff };
		sync_count++;
		return send(0x080, data, 2);
	}


	/**
	 * Sends the CANopen NMT message.
	 * @param state The state to transition to.
	 */
	uint32_t nmt(uint8_t state)
	{
		uint8_t data[] = { state, 0x00 };
		return send(0x000, data, 2);
	}


	/**
	 * Sends an SDO request message.
	 * @param address The address ID of the receiver.
	 * @param index The SDO index.
	 * @param subindex The SDO subindex.
	 * @param value The optional data (1-4 bytes).
	 * @param size The number of value bytes.
	 * @returns 0 on success; otherwise the error value.
	 */
	uint32_t sdo(uint16_t cob, uint16_t index, uint8_t subindex=0, uint32_t value=0, uint8_t size=0)
	{
		cob += role==Master ? 0x600 : 0x580;
		uint8_t data[] = {
				//size == 0 ? 0x40 : 0x2f-size,
				size == 1 ? 0x2f : size == 2 ? 0x2b : size == 4 ? 0x23 : 0x40,
				index & 0xff,
				index >> 8,
				subindex,
				value & 0x000000ff,
				(value & 0x0000ff00) >> 8,
				(value & 0x00ff0000) >> 16,
				(value & 0xff000000) >> 24
		};
		uint32_t error = send(cob, data, size == 0 ? 8 : 4 + size);
		return error;
	}


	/**
	 * Sends an PDO request message.
	 * @param address The address ID of the receiver.
	 * @param index The SDO index.
	 * @param subindex The SDO subindex.
	 */
	uint32_t pdo(uint16_t address, uint16_t index, uint8_t subindex=0, uint32_t value=0, uint8_t size=0)
	{
		uint8_t data[] = {
				index & 0xff,
				index >> 8,
				subindex,
				value & 0x000000ff,
				(value & 0x0000ff00) >> 8,
				(value & 0x00ff0000) >> 16,
				(value & 0xff000000) >> 24,
				0
		};
		uint32_t error = send(address, data, 3 + size);
		return error;
	}


	/**
	 * Called by interrupt routine to handle a received message.
	 */
	void on_message(void) override
	{
		CAN_RxHeaderTypeDef can_rx_header;
		uint8_t data[8];
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can_rx_header, data);
		uint16_t cob = can_rx_header.StdId;
		uint16_t node = cob_to_node(cob);

		if (cob == 0x000)
		{
			if (callback != nullptr)
				callback->on_nmt(*data);
		}

		else if ((cob & 0x700) == 0x700)
		{
			if (callback != nullptr)
				callback->on_heartbeat(node);
		}

		else if ((cob & 0x580) == 0x580)
		{
			uint16_t index = lsb_uint16_to_uint16(data+1);
			uint8_t subindex = data[3];
			if (callback != nullptr)
				callback->on_sdo(node, index, subindex, data+4);
		}

		else if ((cob & 0x180) == 0x180 || (cob & 0x280) == 0x280 || (cob & 0x380) == 0x380 || (cob & 0x480) == 0x480 || (cob & 0x190) == 0x190 || (cob & 0x290) == 0x290)
		{
			if (callback != nullptr)
				callback->on_pdo(cob, data);
		}

		else
		{
			if (callback != nullptr)
				callback->on_other_message(cob, data);
		}

		if (message_callback != nullptr)
			message_callback(cob, data);
	}


	/**
	 * Registers the instance of the class that implements ICanOpenCallback.
	 */
	void set_callback(CanOpen::ICanOpenCallback* instance)
	{
		this->callback = instance;
	}

	void set_message_callback(fastdelegate::FastDelegate2<uint16_t, uint8_t*> callback)
	{
		this->message_callback = callback;
	}


	/**
	 * Extracts the node ID from a COB.
	 * @param cob The COB.
	 * @returns The node ID.
	 */
	uint8_t cob_to_node(uint16_t cob)
	{
		if (allow_tpdo5)
		{
		    if ((cob & 0x90) == 0x80) cob -= 0x10;
		    return cob & 0x3f;
		}
		return cob & 0x7f;
	}

	/**
	 * Converts LSB word into uint16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static uint16_t lsb_uint16_to_uint16(uint8_t* data)
	{
		return data[0] | data[1] << 8;
	}


	/**
	 * Converts uint8_t pointer to uint8_t.
	 * @param data Pointer to the byte.
	 * @return The value.
	 */
	static uint8_t lsb_uint8_to_uint8(uint8_t* data)
	{
		return *data;
	}


	/**
	 * Converts uint8_t pointer to uint8_t.
	 * @param data Pointer to the byte.
	 * @return The value.
	 */
	static int8_t lsb_int8_to_int8(uint8_t* data)
	{
		return *data;
	}


    /**
     * Converts LSB word into int16.
     * @param data Pointer to the first byte of the word.
     * @return The value.
     */
	static int16_t lsb_int16_to_int16(uint8_t* data)
	{
		return data[0] | data[1] << 8;
	}


	/**
	 * Converts LSB word into uint16.
	 * @param data Pointer to the first byte of the word.
	 * @return The value.
	 */
	static uint32_t lsb_uint32_to_uint32(uint8_t* data)
	{
		return data[0] | data[1] << 8 | data[2] << 16 | data[3];
	}


    /**
     * Converts LSB word into int16.
     * @param data Pointer to the first byte of the word.
     * @return The value.
     */
	static int32_t lsb_int32_to_int32(uint8_t* data)
	{
		return data[0] | data[1] << 8 | data[2] << 16 | data[3];
	}


    /**
     * Converts LSB word with implicit decimal into float.
     * @param data Pointer to the first byte of the word.
     * @param divisor Value to divide the integer value by to translate it to float.
     *        The default is 1 (zero decimal places).
     * @return The value.
     */
	static float lsb_int16_to_float(uint8_t* data, uint16_t divisor=1)
	{
		int16_t u = lsb_int16_to_int16(data);
		return ((float)u) / (float)divisor;
	}


    /**
     * Converts LSB word with implicit decimal into float.
     * @param data Pointer to the first byte of the word.
     * @param divisor Value to divide the integer value by to translate it to float.
     *        The default is 1 (zero decimal places).
     * @return The value.
     */
	static float lsb_uint16_to_float(uint8_t* data, uint16_t divisor=1)
	{
		uint16_t u = lsb_uint16_to_uint16(data);
		return ((float)u) / (float)divisor;
	}


    /**
     * Converts LSB double word with implicit decimal into float.
     * @param data Pointer to the first byte of the word.
     * @param divisor Value to divide the integer value by to translate it to float.
     *        The default is 1 (zero decimal places).
     * @return The value.
     */
	static float lsb_uint32_to_float(uint8_t* data, uint16_t divisor=1)
	{
		uint32_t u = lsb_uint32_to_uint32(data);
		return ((float)u) / (float)divisor;
	}


    /**
     * Converts byte sequence into a string.
     * @param data Pointer to the first byte.
     * @param dest Pointer to the destination.
     * @param length The number of characyters, excluding the NUL termination.
     * @return The value.
     */
	static void bytes_to_string(uint8_t* data, char* dest, uint8_t length, bool terminate=true)
	{
		for (uint8_t i=0; i < length; i++)
			dest[i] = (char)data[length-1-i];
		if (terminate)
			dest[length] = '\0';
	}


private:
	uint32_t sync_count = 0;
	ICanOpenCallback* callback = nullptr;
	roles role = Master;
	fastdelegate::FastDelegate2<uint16_t, uint8_t*> message_callback;
	bool allow_tpdo5 = false;
};


#endif /* INC_COMMS_CANOPEN_H_ */
