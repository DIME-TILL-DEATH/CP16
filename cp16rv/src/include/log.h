/*
 * log.h
 * The basic idea is to exploit Pade polynomials.
 * A lot of ideas were inspired by the cephes math library (by Stephen L. Moshier
 * moshier@na-net.ornl.gov) as well as actual code. 
 * The Cephes library can be found here:  http://www.netlib.org/cephes/
 * 
 *  Created on: Jun 23, 2012
 *      Author: Danilo Piparo, Thomas Hauth, Vincenzo Innocente
 */

/* 
 * VDT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOG_H_
#define LOG_H_

#include "vdtcore_common.h"


namespace vdt{

// Log single precision --------------------------------------------------------

namespace details{

const double LOG_UPPER_LIMIT_F = VDT_MAX_F;
const double LOG_LOWER_LIMIT_F = VDT_MIN_F;

const float PX1logf = 7.0376836292E-2f;
const float PX2logf	= -1.1514610310E-1f;
const float PX3logf	= 1.1676998740E-1f;
const float PX4logf	= -1.2420140846E-1f;
const float PX5logf	= 1.4249322787E-1f;
const float PX6logf	= -1.6668057665E-1f;
const float PX7logf	= 2.0000714765E-1f;
const float PX8logf	= -2.4999993993E-1f;
const float PX9logf	= 3.3333331174E-1f;

inline float get_log_poly(const float x){
	float y = x*PX1logf;
	y += PX2logf;
	y *= x;
	y += PX3logf;
	y *= x;
	y += PX4logf;
	y *= x;
	y += PX5logf;
	y *= x;
	y += PX6logf;
	y *= x;
	y += PX7logf;
	y *= x;
	y += PX8logf;
	y *= x;
	y += PX9logf;
	return y;
}

const float SQRTHF = 0.707106781186547524f;

}

// Log single precision --------------------------------------------------------
inline float fast_logf( float x ) {

	const float original_x = x;

	float fe;
	x = details::getMantExponentf( x, fe);

	x > details::SQRTHF? fe+=1.f : x+=x ;
	x -= 1.0f;

	const float x2 = x*x;

	float res = details::get_log_poly(x);
	res *= x2*x;

	res += -2.12194440e-4f * fe;
	res +=  -0.5f * x2;

	res= x + res;

	res += 0.693359375f * fe;

	if (original_x > details::LOG_UPPER_LIMIT_F)
		res = std::numeric_limits<float>::infinity();
	if (original_x < details::LOG_LOWER_LIMIT_F)
		res = -std::numeric_limits<float>::quiet_NaN();

	return res;
}


} //vdt namespace

#endif /* LOG_H_ */
