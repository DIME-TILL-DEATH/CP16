#ifndef __APPDEFS_H__
#define __APPDEFS_H__

#include "sdk.h"

inline void i2hex(uint8_t val, char* dest)
{
	static const char hexes[] = "0123456789abcdef";
	dest[0] = hexes[ val >> 4  ];
	dest[1] = hexes[ val & 0xf ];
	dest[2] = 0;
}

#endif /*__APPDEFS_H__*/
