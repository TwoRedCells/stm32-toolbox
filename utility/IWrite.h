/**
 * @file 		utility/IWrite.g
 * @class		IWrite
 * @author 		Yvan Rodrigues <yvan.r@radskunkworks.com>
 * @brief 		Defines an interface for a class that can write a byte.
 * @date 		2024-02-09
 * @copyright 	Copyright (c) 2024 Robotic Assistance Devices
 */

#ifndef UTILITY_IWRITE_H
#define UTILITY_IWRITE_H


class IWrite
{
public:
	virtual size_t write(uint8_t value) = 0;
};

#endif
