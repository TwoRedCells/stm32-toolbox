/*
 * StringSumHelper.h
 *
 *  Created on: Sep 22, 2023
 *      Author: YvanRodriguez
 */

#ifndef LIB_STM32_TOOLBOX_COMMS_ETHERNET_STRINGSUMHELPER_H_
#define LIB_STM32_TOOLBOX_COMMS_ETHERNET_STRINGSUMHELPER_H_


class StringSumHelper : public String
{
public:
	StringSumHelper(const String &s) : String(s) {}
	StringSumHelper(const char *p) : String(p) {}
	StringSumHelper(char c) : String(c) {}
	StringSumHelper(unsigned char num) : String(num) {}
	StringSumHelper(int num) : String(num) {}
	StringSumHelper(unsigned int num) : String(num) {}
	StringSumHelper(long num) : String(num) {}
	StringSumHelper(unsigned long num) : String(num) {}
	StringSumHelper(float num) : String(num) {}
	StringSumHelper(double num) : String(num) {}
};



#endif /* LIB_STM32_TOOLBOX_COMMS_ETHERNET_STRINGSUMHELPER_H_ */
