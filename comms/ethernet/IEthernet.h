#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_IETHERNET_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_IETHERNET_H_


class IEthernet
{
	virtual uint16_t Read(uint16_t _addr, uint8_t _cb, void* buf, uint16_t len) = 0;

	virtual void Write(uint16_t _addr, uint8_t _cb, const void *_buf, uint16_t _len) = 0;
};


#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_IETHERNET_H_ */
