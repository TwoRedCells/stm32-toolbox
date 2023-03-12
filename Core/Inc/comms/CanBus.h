/**
 * \file       comms/CanBus.h
 * \class      CanBus
 * \brief      Encapsulates CAN bus communications.
 */

#ifndef INC_COMMS_CANBUS_H_
#define INC_COMMS_CANBUS_H_

extern CAN_HandleTypeDef hcan;

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
	 * @param returns 0 on success; otherwise the error code.
	 */
	uint32_t setup(void)
	{
		can_tx_header.IDE = CAN_ID_STD; // 11-bit ID
		can_tx_header.RTR = CAN_RTR_DATA; // Normal data
		can_tx_header.TransmitGlobalTime = DISABLE;

		CAN_FilterTypeDef can_filter;
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
			return HAL_CAN_GetError(hcan);

		if (HAL_CAN_Start(hcan) != HAL_OK)
			return HAL_CAN_GetError(hcan);

		if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
			return HAL_CAN_GetError(hcan);
	}


//	/**
//	 * Polls the receive buffer, and reads data if it is available.
//	 */
//	void loop(void)
//	{
//		while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0)
//		{
//			HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can_rx_header, data);
//			//log_d("CAN received %u bytes for %x: %x %x %x %x %x %x %x %x", can_rx_header.DLC, can_rx_header.StdId, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
//			on_message(can_rx_header.StdId, data, can_rx_header.DLC);
//			//		osDelay(5);
//		}
//	}

	bool is_data(void)
	{
		return (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0);
	}

	uint32_t poll(CAN_RxHeaderTypeDef* can_rx_header, uint8_t* data)
	{
		if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, can_rx_header, data) != HAL_OK)
			return HAL_CAN_GetError(hcan);
	}


	/**
	 * Sends data on the bus.
	 * @param address The recipient address.
	 * @param data Pointer to the data bytes.
	 * @param length Length of the message (maximum 8).
	 */
	uint32_t send(uint16_t address, uint8_t* data, uint8_t length)
	{
		can_tx_header.DLC = length;
		can_tx_header.StdId = address;
		uint8_t ret = HAL_CAN_AddTxMessage(hcan, &can_tx_header, data, &can_tx_mailbox);

		if(ret != HAL_OK)
			return HAL_CAN_GetError(hcan);

		return 0;
	}


	void reset_error(void)
	{
		HAL_CAN_ResetError(hcan);
	}


	/**
	 * Called by interrupt routine to handle a received message.
	 */
	virtual void on_message(void)
	{
		CAN_RxHeaderTypeDef can_rx_header;
		uint8_t data[8];
		HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &can_rx_header, data);
	}
protected:
	CAN_HandleTypeDef *hcan;


private:
	CAN_TxHeaderTypeDef can_tx_header;
	uint32_t can_tx_mailbox;
//	uint8_t data[8] = { 0 };
};


#endif /* INC_COMMS_CANBUS_H_ */
