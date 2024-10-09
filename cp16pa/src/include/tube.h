#ifndef __TUBE_H__
#define __TUBE_H__

#include "vdt/pow.h"

// test condition [-1.0....1.0]

inline float general_tube_amplitude(float grid_voltage)
{
	float tmp ;
	if ( grid_voltage < -1.0 )
		tmp = -0.705 ;
	else
	   if ( grid_voltage < 0.886 )
		   tmp =  vdt::fast_powf( 0.7*(grid_voltage+1.0) , 1.5 ) - 0.705;
	   else
		   tmp = 1.612 - 0.705 - 2.0 / vdt::fast_powf( grid_voltage + 0.31, 17 ) ;

	return tmp ;
}

#endif /* __TUBE_H__ */
