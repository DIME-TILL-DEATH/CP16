/*
 * tan.h
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

#ifndef TAN_H_
#define TAN_H_

#include "vdtcore_common.h"
#include "sincos.h"

namespace vdt{


namespace details{

const double PX1tan=-1.30936939181383777646E4;
const double PX2tan=1.15351664838587416140E6;
const double PX3tan=-1.79565251976484877988E7;

const double QX1tan = 1.36812963470692954678E4;
const double QX2tan = -1.32089234440210967447E6;
const double QX3tan = 2.50083801823357915839E7;
const double QX4tan = -5.38695755929454629881E7;

const double DP1tan = 7.853981554508209228515625E-1;
const double DP2tan = 7.94662735614792836714E-9;
const double DP3tan = 3.06161699786838294307E-17;

const float DP1Ftan = 0.78515625;
const float DP2Ftan = 2.4187564849853515625e-4;
const float DP3Ftan = 3.77489497744594108e-8;


//------------------------------------------------------------------------------
/// Reduce to -45 to 45
inline double reduce2quadranttan(double x, int32_t& quad) {

    x = details::abs(x);
    quad = int( VDT_4_PI * x ); // always positive, so (int) == std::floor
    quad = (quad+1) & (~1);
    const double y = quad;
    // Extended precision modular arithmetic
    return ((x - y * DP1tan) - y * DP2tan) - y * DP3tan;
  }

//------------------------------------------------------------------------------
/// Reduce to -45 to 45
inline float reduce2quadranttan(float x, int32_t& quad) {

    x = details::absf(x);
    quad = int( VDT_4_PI_F * x ); // always positive, so (int) == std::floor
    quad = (quad+1) & (~1);
    const float y = quad;
    // Extended precision modular arithmetic
    return ((x - y * DP1Ftan) - y * DP2Ftan) - y * DP3Ftan;
  }

}

//------------------------------------------------------------------------------
/// Double precision tangent implementation
inline double fast_tan(double x){

    const uint64_t sign_mask = details::getSignMask(x);

    int32_t quad =0;
    const double z=details::reduce2quadranttan(x,quad);

    const double zz = z * z;

    double res=z;

    if( zz > 1.0e-14 ){
        double px = details::PX1tan;
        px *= zz;
        px += details::PX2tan;
        px *= zz;
        px += details::PX3tan;

        double qx=zz;
        qx += details::QX1tan;
        qx *=zz; 
        qx += details::QX2tan;
        qx *=zz;
        qx += details::QX3tan;
        qx *=zz;
        qx += details::QX4tan;

        res = z + z * zz * px / qx;
    }

    // A no branching way to say: if j&2 res = -1/res. You can!!!
    quad &=2;
    quad >>=1;
    const int32_t alt = quad^1;
    // Avoid fpe generated by 1/0 if res is 0
    const double zeroIfXNonZero = (x==0.);
    res += zeroIfXNonZero;
    res = quad * (-1./res) + alt * res; // one coeff is one and one is 0!

    // Again, return 0 if res==0, the correct result otherwhise
    return details::dpXORuint64(res,sign_mask) * (1.-zeroIfXNonZero);

}

// Single precision ------------------------------------------------------------

inline float fast_tanf(float x){
    const uint32_t sign_mask = details::getSignMask(x);

    int32_t quad =0;
    const float z=details::reduce2quadranttan(x,quad);

    const float zz = z * z;

    float res=z;

    if( zz > 1.0e-14f ){
      res =
        ((((( 9.38540185543E-3f * zz
        + 3.11992232697E-3f) * zz
        + 2.44301354525E-2f) * zz
        + 5.34112807005E-2f) * zz
        + 1.33387994085E-1f) * zz
        + 3.33331568548E-1f) * zz * z
        + z;
    }

    // A no branching way to say: if j&2 res = -1/res. You can!!!
    quad &=2;
    quad >>=1;
    const int32_t alt = quad^1;
    // Avoid fpe generated by 1/0 if res is 0
    const float zeroIfXNonZero = (x==0.f);
    res += zeroIfXNonZero;
    res = quad * (-1.f/res) + alt * res; // one coeff is one and one is 0!

    return details::spXORuint32(res,sign_mask) * (1.f-zeroIfXNonZero);

}

} //vdt namespace


#endif /* COS_H_ */
