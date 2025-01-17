/*
 * atan2.h
 * The basic idea is to exploit Pade polynomials.
 * A lot of ideas were inspired by the cephes math library (by Stephen L. Moshier
 * moshier@na-net.ornl.gov) as well as actual code.
 * The Cephes library can be found here:  http://www.netlib.org/cephes/
 *
 *  Created on: Sept 20, 2012
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

#ifndef ATAN2_H_
#define ATAN2_H_

#include "vdtcore_common.h"
#include "atan.h"

namespace vdt{

inline double fast_atan2( double y, double x ) {
    // move in first octant
    double xx = details::abs(x);
    double yy = details::abs(y);
    double tmp (0.0);
    if (yy>xx) {
        tmp = yy;
        yy=xx; xx=tmp;
        tmp=1.;
    }

    // To avoid the fpe, we protect against /0.
    const double oneIfXXZero = (xx==0.);
    double t=yy/(xx+oneIfXXZero);
    double z=t;

    double s = details::VDT_PI_4;
    double factor = details::MOREBITSO2;
    t = (t-1.0) / (t+1.0);

    if( z > details::T3PO8 ) {
        s = details::VDT_PI_2;
        factor = details::MOREBITS;
        t = -1.0 / z ;
    }
    if ( z <= 0.66 ) {
        s = 0.;
        factor = 0.;
        t = z;
    }

    const double t2 = t * t;

    const double px = details::get_atan_px(t2);
    const double qx = details::get_atan_qx(t2);

    //double res = y +x * x2 * px / qx + x +factor;

    const double poq=px / qx;

    double ret = t * t2 * poq + t;
    ret+=s;

    ret+=factor;

    // Here we put the result to 0 if xx was 0, if not nothing happens!
    ret*= (1.-oneIfXXZero);

    // move back in place
    if (y==0) ret=0.0;
    if (tmp!=0) ret = details::VDT_PI_2 - ret;
    if (x<0) ret = details::VDT_PI - ret;
    if (y<0) ret = -ret;


    return ret;
}

inline float fast_atan2f( float y, float x ) {

    // move in first octant
    float xx = details::absf(x);
    float yy = details::absf(y);
    float tmp (0.0f);
    if (yy>xx) {
        tmp = yy;
        yy=xx; xx=tmp;
        tmp =1.f;
    }

    // To avoid the fpe, we protect against /0.
    const float oneIfXXZero = (xx==0.f);
    
    float t=yy/(xx/*+oneIfXXZero*/);
    float z=t;
    if( t > 0.4142135623730950f ) // * tan pi/8
            z = (t-1.0f)/(t+1.0f);

    //printf("%e %e %e %e\n",yy,xx,t,z);
    float z2 = z * z;
    
    float ret =(((( 8.05374449538e-2f * z2
                    - 1.38776856032E-1f) * z2
                    + 1.99777106478E-1f) * z2
                    - 3.33329491539E-1f) * z2 * z
                    + z );

    // Here we put the result to 0 if xx was 0, if not nothing happens!
    ret*= (1.f - oneIfXXZero);
    
    // move back in place
    if (y==0.f) ret=0.f;
    if( t > 0.4142135623730950f ) ret += details::VDT_PI_4_F;
    if (tmp!=0) ret = details::VDT_PI_2_F - ret;
    if (x<0.f) ret = details::VDT_PI_F - ret;
    if (y<0.f) ret = -ret;

    return ret;

}

} // end namespace vdt


#endif
