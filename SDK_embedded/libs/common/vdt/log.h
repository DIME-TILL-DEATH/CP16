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

// local namespace for the constants/functions which are necessary only here
namespace details{

const double LOG_UPPER_LIMIT = VDT_MAX;
const double LOG_LOWER_LIMIT = VDT_MIN;

const double SQRTH = 0.70710678118654752440;

inline double get_log_px(const double x){
	const double PX1log = 1.01875663804580931796E-4;
	const double PX2log = 4.97494994976747001425E-1;
	const double PX3log = 4.70579119878881725854E0;
	const double PX4log = 1.44989225341610930846E1;
	const double PX5log = 1.79368678507819816313E1;
	const double PX6log = 7.70838733755885391666E0;

	double px = PX1log;
	px *= x;
	px += PX2log;
	px *= x;
	px += PX3log;
	px *= x;
	px += PX4log;
	px *= x;
	px += PX5log;
	px *= x;
	px += PX6log;
	return px;

}

inline double get_log_qx(const double x){
	const double QX1log = 1.12873587189167450590E1;
	const double QX2log = 4.52279145837532221105E1;
	const double QX3log = 8.29875266912776603211E1;
	const double QX4log = 7.11544750618563894466E1;
	const double QX5log = 2.31251620126765340583E1;

	double qx = x;
	qx += QX1log;
	qx *=x;
	qx += QX2log;
	qx *=x;
	qx += QX3log;
	qx *=x;
	qx += QX4log;
	qx *=x;
	qx += QX5log;
	return qx;
}

}

// Log double precision --------------------------------------------------------
inline double fast_log(double x){

	const double original_x = x;

	/* separate mantissa from exponent */
	double fe;
	x = details::getMantExponent(x,fe);

	// blending
	x > details::SQRTH? fe+=1. : x+=x ;
	x -= 1.0;

	/* rational form */
	double px =  details::get_log_px(x);

	//for the final formula
	const double x2 = x*x;
	px *= x;
	px *= x2;

	const double qx = details::get_log_qx(x);

	double res = px / qx ;

	res -= fe * 2.121944400546905827679e-4;
	res -= 0.5 * x2  ;

	res = x + res;
	res += fe * 0.693359375;

	if (original_x > details::LOG_UPPER_LIMIT)
		res = std::numeric_limits<double>::infinity();
	if (original_x < details::LOG_LOWER_LIMIT) // THIS IS NAN!
		res =  - std::numeric_limits<double>::quiet_NaN();

	return res;

}

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


// Внутренние функции действительно быстры, но все еще недостаточны для подлинно кросс-платформенной,
// независимой от компилятора реализации log2. Поэтому необходим быстрый, не имеющий ветвлений, абстрактный алгоритм.

// The part of rounding down to the next lower power of 2 was taken from Power-of-2 Boundaries and the part
// of getting the number of trailing zeros was taken from BitScan (the (bb & -bb) code there is to single out
// the rightmost bit that is set to 1, which is not need after we've rounded the value down to the next power of 2).
// And the 32-bit implementation, by the way, is

inline uint32_t log2 (uint64_t val)
{
   #if 0

   static const int lookup[64] =
	                  {
                            63,  0, 58,  1, 59, 47, 53,  2,
                            60, 39, 48, 27, 54, 33, 42,  3,
                            61, 51, 37, 40, 49, 18, 28, 20,
                            55, 30, 34, 11, 43, 14, 22,  4,
                            62, 57, 46, 52, 38, 26, 32, 41,
                            50, 36, 17, 19, 29, 10, 13, 21,
                            56, 45, 25, 31, 35, 16,  9, 12,
                            44, 24, 15,  8, 23,  7,  6,  5
                          };
    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    value |= value >> 32;
    return lookup[((uint64_t)((value - (value >> 1))*0x07EDD5E59A4E28C2)) >> 58];

    #else
       return __builtin_ffsll(val) - 1;
    #endif
}




inline uint32_t log2 (uint32_t val)
{
  #if 0

  static const int lookup[32] =
      {
        0,  9,  1, 10, 13, 21,  2, 29,
       11, 14, 16, 18, 22, 25,  3, 30,
       08, 12, 20, 28, 15, 17, 24,  7,
       19, 27, 23,  6, 26,  5,  4, 31
      };

    value |= value >> 1;
    value |= value >> 2;
    value |= value >> 4;
    value |= value >> 8;
    value |= value >> 16;
    return lookup[(uint32_t)(value*0x07C4ACDD) >> 27];

    #else

    return __builtin_ffsl(val) - 1;

    #endif
}


} //vdt namespace

#endif /* LOG_H_ */
