/**
 * @file 		comms/http/HttpHandler.h
 * @class		Httphandler
 * @author 		Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief 		Classes can subclass in order to provide code that handles HTTP requests.
 * @date 		2022-09-15
 * @copyright 	Copyright (c) 2022 Robotic Assistance Devices
 */

#ifndef INC_STM32_TOOLBOX_COMMS_HTTPHANDLER_H_
#define INC_STM32_TOOLBOX_COMMS_HTTPHANDLER_H_

#include <comms/ethernet/EthernetClient.h>


/**
 * @brief Classes can subclass in order to provide code that handles HTTP requests.
 */
class HttpHandler
{
public:
	virtual bool can_handle(String uri) = 0; /// Implementers should evaluate the passed URI, and return true if they are able to handle it.
	virtual uint16_t handle(String uri, EthernetClient *client) = 0;  /// Implementers should handle the request to the specified URI.
};

#endif /* INC_STM32_TOOLBOX_COMMS_HTTPHANDLER_H_ */
