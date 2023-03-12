/**
 * \file       comms/CanBus.h
 * \class      CanBus
 * \brief      Encapsulates CAN bus communications.
 */

#ifndef INC_COMMS_CANBUS_H_
#define INC_COMMS_CANBUS_H_

#include "diagnostics/Log.h"
#include "globals.hpp"


class CanBus
{
public:
	bool is_alive = false;

	/**
	 * Creates a \ref CanBus instance.
	 * @param hcan Handle to the hardware resource.
	 */
	CanBus(CAN_HandleTypeDef *hcan)
	{
		this->hcan = hcan; // Defined globally.
	}


	/**
	 * Performs initialization tasks.
	 * @param bitrate The bus bitrate for communications, generally a multiple of 125000.
	 */
	void setup(uint32_t bitrate)
	{
		log_d("CAN starting.");
		is_initialized = true;
		is_alive = false;
		this->bitrate = bitrate;
		hcan->Instance = CAN1;
		hcan->Init.Prescaler = 2000000 / bitrate;
		hcan->Init.Mode = CAN_MODE_NORMAL;
		hcan->Init.SyncJumpWidth = CAN_SJW_4TQ;
		hcan->Init.TimeSeg1 = CAN_BS1_11TQ;
		hcan->Init.TimeSeg2 = CAN_BS2_4TQ;
		hcan->Init.TimeTriggeredMode = DISABLE;
		hcan->Init.AutoBusOff = DISABLE;
		hcan->Init.AutoWakeUp = DISABLE;
		hcan->Init.AutoRetransmission = DISABLE;
		hcan->Init.ReceiveFifoLocked = DISABLE;
		hcan->Init.TransmitFifoPriority = DISABLE;
		if (HAL_CAN_Init(hcan) != HAL_OK)
		{
			log_e("HAL_CAN_Init FAILED");
			fault.alert(Fault::CANCommunicationsOpen);
			is_initialized = false;
			return;
		}

		can_tx_header.IDE = CAN_ID_STD; // 11-bit ID
		can_tx_header.RTR = CAN_RTR_DATA; // Normal data
		can_tx_header.TransmitGlobalTime = DISABLE;

		can_filter.FilterBank = 0;
		can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
		can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
		can_filter.FilterIdHigh = 0x0000;
		can_filter.FilterIdLow = 0x0000;
		can_filter.FilterMaskIdHigh = 0x0000;
		can_filter.FilterMaskIdLow = 0x0000;
		can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
		can_filter.FilterActivation = CAN_FILTER_ENABLE;
		can_filter.SlaveStartFilterBank = 14;
		if (HAL_CAN_ConfigFilter(hcan, &can_filter) != HAL_OK)
		{
			log_e("HAL_CANConfigFilter FAILED");
			fault.alert(Fault::CANCommunicationsOpen);
			is_initialized = false;
			return;
		}

		if (HAL_CAN_Start(hcan) != HAL_OK)
		{
			log_e("HAL_CAN_Start FAILED");
			fault.alert(Fault::CANCommunicationsOpen);
			is_initialized = false;
			return;
		}

	/*	if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
		{
			log_e("HAL_CAN_ActivateNotification FAILED");
			fault.alert(fault.BatteryCommunications);
			initialized = false;
			return;
		}*/
		log_d("CAN started.");
		fault.reset(Fault::CANCommunicationsOpen);
	}


	/**
	 * Polls the receive buffer, and reads data if it is available.
	 */
	void loop(void)
	{
		while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0)
		{
			HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can_rx_header, data);
			//log_d("CAN received %u bytes for %x: %x %x %x %x %x %x %x %x", can_rx_header.DLC, can_rx_header.StdId, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
			on_message(can_rx_header.StdId, data, can_rx_header.DLC);
			osDelay(5);
		}
	}


	/**
	 * Sends data on the bus.
	 * @param address The recipient address.
	 * @param data Pointer to the data bytes.
	 * @param length Length of the message (maximum 8).
	 */
	void send(uint16_t address, uint8_t* data, uint8_t length)
	{
		if (is_initialized)
		{
			can_tx_header.DLC = length;
			can_tx_header.StdId = address;
			uint8_t ret = HAL_CAN_AddTxMessage(hcan, &can_tx_header, data, &can_tx_mailbox);

			if(ret != HAL_OK)
			{
				uint32_t error = HAL_CAN_GetError(hcan);
				log_e("HAL_CAN_AddTxMessage FAILED (%x%x): %u bytes for %x: %x %x %x %x %x %x %x %x", error>>16, error & 0xffff, can_tx_header.DLC, can_tx_header.StdId, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
				fault.alert(Fault::CANCommunicationsTransmit);
				return;
			}
			fault.reset(Fault::CANCommunicationsTransmit);
		}
		else
		{
			log_i("Retrying CAN initialization.");
			setup(bitrate);
		}
	}

protected:
	/**
	 * Implement in a subclass to process messages received on the bus.
	 * @param address
	 * @param data
	 * @param length
	 */
	virtual void on_message(uint16_t address, uint8_t* data, uint8_t length)
	{
		is_alive = true;
	}


private:
	CAN_TxHeaderTypeDef can_tx_header;
	CAN_RxHeaderTypeDef can_rx_header;
	uint32_t can_tx_mailbox;
	CAN_FilterTypeDef can_filter; //declare CAN filter structure
	bool is_initialized = false;
	CAN_HandleTypeDef *hcan;
	uint8_t data[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	uint32_t bitrate;
};


#endif /* INC_COMMS_CANBUS_H_ */
