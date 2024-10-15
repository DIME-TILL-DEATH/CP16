/*
 * vdtcore_common.h
 * Common functions for the vdt routines.
 * The basic idea is to exploit Pade polynomials.
 * A lot of ideas were inspired by the cephes math library (by Stephen L. Moshier
 * moshier@na-net.ornl.gov) as well as actual code for the exp, log, sin, cos, 
 * tan, asin, acos and atan functions. The Cephes library can be found here:
 * http://www.netlib.org/cephes/
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

#ifndef VDTCOMMON_H_
#define VDTCOMMON_H_

#include <inttypes.h>
#include <limits>

namespace vdt{

namespace details{

  const double MOREBITS = 6.123233995736765886130E-17;

  //e
  const double VDT_E =         2.71828182845904523536 ;
  // log2(e)
  const double VDT_LOG2E =     1.44269504088896340736 ;
  //log10(e)
  const double VDT_LOG10E =    0.434294481903251827651 ;
  // ln(2)
  const double VDT_LN2 =       0.693147180559945309417 ;
  // ln(10)
  const double VDT_LN10 =      2.30258509299404568402 ;
  //pi
  const double VDT_PI =        3.14159265358979323846 ;
  // pi/2
  const double VDT_PI_2 =      1.57079632679489661923 ;
  // pi/4
  const double VDT_PI_4 =      0.785398163397448309616 ;

  // 1/(2*pi)
  const double VDT_1_2PI =     0.1591549430918953 ;
  // 1/pi
  const double VDT_1_PI =      0.318309886183790671538 ;
  // 2/pi
  const double VDT_2_PI =      0.636619772367581343076 ;
  // 4/pi
  const double VDT_4_PI =      4.0/VDT_PI ;

  // 2/sqrt(pi)
  const double VDT_2_SQRTPI =  1.12837916709551257390 ;
  // sqrt(2)
  const double VDT_SQRT2 =     1.41421356237309504880 ;
  // 1/sqrt(2)
  const double VDT_1_SQRT2 =   0.707106781186547524401 ;
    // sqrt(3)
  const double VDT_SQRT3   =   1.732050807568877293527 ;
  // sqrt(3)/2
  const double VDT_SQRT3_2 =   1.732050807568877293527 / 2 ;
  // 1/sqrt(3)
  const double VDT_1_SQRT3 =   1.0/VDT_SQRT3 ;

  // 2*pi
  const double VDT_2PI  =      2.0*VDT_PI;
  //  1/360
  const double VDT_1_360 =     1.0 / 360 ;

  // rad to degree
  const double VDT_R2G   =     360.0 * VDT_1_2PI;
  // degree to rad
  const double VDT_G2R   =     VDT_2PI / 360.0;

  const double VDT_EPSILON = __DBL_EPSILON__;

  const double VDT_MAX     = __DBL_MAX__;
  const double VDT_MIN     = __DBL_MIN__;



  //e
  const float VDT_E_F =        VDT_E ;
  // log2(e)
  const float VDT_LOG2E_F =    VDT_LOG2E ;
  //log10(e)
  const float VDT_LOG10E_F =   VDT_LOG10E ;
  // ln(2)
  const float VDT_LN2_F =      VDT_LN2 ;
  // ln(10)
  const float VDT_LN10_F =     VDT_LN10 ;
  //pi
  const float VDT_PI_F =       VDT_PI ;
  // pi/2
  const float VDT_PI_2_F =     VDT_PI_2 ;
  // pi/4
  const float VDT_PI_4_F =     VDT_PI_4 ;
  // 1/(2*pi)
  const float VDT_1_2PI_F =    VDT_1_2PI ;
  // 1/pi
  const float VDT_1_PI_F =     VDT_1_PI ;
  // 2/pi
  const float VDT_2_PI_F =     VDT_2_PI ;
  // 4/pi
  const float VDT_4_PI_F =     VDT_4_PI ;

  // 2/sqrt(pi)
  const float VDT_2_SQRTPI_F = VDT_2_SQRTPI ;
  // sqrt(2)
  const float VDT_SQRT2_F =    VDT_SQRT2 ;
  // 1/sqrt(2)
  const float VDT_1_SQRT2_F =  VDT_1_SQRT2 ;
    // sqrt(3)
  const float VDT_SQRT3_F   =  VDT_SQRT3 ;
  // sqrt(3)/2
  const float VDT_SQRT3_2_F=   VDT_SQRT3_2 ;
  // 1/sqrt(3)
  const float VDT_1_SQRT3_F =  VDT_1_SQRT3 ;
  // 2*pi
  const float VDT_2PI_F  =     VDT_2PI ;
  //  1/360
  const float VDT_1_360_F =    VDT_1_360 ;

  // rad to degree
  const float VDT_R2G_F =      VDT_R2G;
  // degree to rad
  const float VDT_G2R_F =      VDT_G2R;

  const float VDT_EPSILON_F = __FLT_EPSILON__;
  const float VDT_MAX_F     = __FLT_MAX__;
  const float VDT_MIN_F     = __FLT_MIN__;

//------------------------------------------------------------------------------

inline float fast_signumf(float val)
{
  return val < 0.0f ? -1.0f : 1.0f  ;
}

// wrap radians arg arountd [-PI...PI)
// warning val mus be less 2*PI
inline float fast_wrap_npi_ppif(float val)
{
  if ( val >=  VDT_PI_F ) { val = - VDT_2PI_F + val; return val ; }
  if ( val <  -VDT_PI_F ) { val =   VDT_2PI_F - val; return val ; }
  return val ;
}

// wrap radians arg arountd [0...2*PI)
// warning val mus be less 4*PI
inline float fast_wrap_zero_2pif(float val)
{
  if ( val >= VDT_2PI_F )
    val -= VDT_2PI_F;
  return val ;
}

//------------------------------------------------------------------------------

/// Used to switch between different type of interpretations of the data (64 bits)
union ieee754{
	ieee754 () {};
	ieee754 (double thed) {d=thed;};
	ieee754 (uint64_t thell) {ll=thell;};
	ieee754 (float thef) {f[0]=thef;};
	ieee754 (uint32_t thei) {i[0]=thei;};
  double d;
  float f[2];
  uint32_t i[2];
  uint64_t ll;
  uint16_t s[4];
};

//------------------------------------------------------------------------------

/// Converts an unsigned long long to a double
inline double uint642dp(uint64_t ll) {
  ieee754 tmp;
  tmp.ll=ll;
  return tmp.d;
}

//------------------------------------------------------------------------------

/// Converts a double to an unsigned long long
inline uint64_t dp2uint64(double x) {
  ieee754 tmp;
  tmp.d=x;
  return tmp.ll;
}

//------------------------------------------------------------------------------
/// Makes an AND of a double and a unsigned long long
inline double dpANDuint64(const double x, const uint64_t i ){
  return uint642dp(dp2uint64(x) & i); 
}
//------------------------------------------------------------------------------
/// Makes an OR of a double and a unsigned long long
inline double dpORuint64(const double x, const uint64_t i ){
  return uint642dp(dp2uint64(x) | i); 
}

/// Makes a XOR of a double and a unsigned long long
inline double dpXORuint64(const double x, const uint64_t i ){
  return uint642dp(dp2uint64(x) ^ i); 
}

//------------------------------------------------------------------------------
inline uint64_t getSignMask(const double x){
  const uint64_t mask=0x8000000000000000ULL;
  return dp2uint64(x) & mask;
}

//------------------------------------------------------------------------------
/// Converts an int to a float
inline float uint322sp(int x) {
    ieee754 tmp;
    tmp.i[0]=x;
    return tmp.f[0];
  }

//------------------------------------------------------------------------------
/// Converts a float to an int
inline uint32_t sp2uint32(float x) {
    ieee754 tmp;
    tmp.f[0]=x;
    return tmp.i[0];
  }

//------------------------------------------------------------------------------
/// Makes an AND of a float and a unsigned long
inline float spANDuint32(const float x, const uint32_t i ){
  return uint322sp(sp2uint32(x) & i); 
}
//------------------------------------------------------------------------------
/// Makes an OR of a float and a unsigned long
inline float spORuint32(const float x, const uint32_t i ){
  return uint322sp(sp2uint32(x) | i); 
}

//------------------------------------------------------------------------------
/// Makes an OR of a float and a unsigned long
inline float spXORuint32(const float x, const uint32_t i ){
  return uint322sp(sp2uint32(x) ^ i); 
}
//------------------------------------------------------------------------------
/// Get the sign mask
inline uint32_t getSignMask(const float x){
  const uint32_t mask=0x80000000;
  return sp2uint32(x) & mask;
}

/// Get the sign mask
inline float getSign(const float x){
  const uint32_t mask=0x80000000;
  const uint32_t float_one=0x3F800000;
  ieee754 tmp;
  tmp.f[0] = x ;
  tmp.i[0] = (tmp.i[0] & mask) | float_one ;
  return tmp.f[0] ;
}

//------------------------------------------------------------------------------
/// Like frexp but vectorising and the exponent is a double.
inline double getMantExponent(const double x, double & fe){

  uint64_t n = dp2uint64(x);

  // Shift to the right up to the beginning of the exponent.
  // Then with a mask, cut off the sign bit
  uint64_t le = (n >> 52);

  // chop the head of the number: an int contains more than 11 bits (32)
  int32_t e = le; // This is important since sums on uint64_t do not vectorise
  fe = e-1023 ;

  // This puts to 11 zeroes the exponent
  n &=0x800FFFFFFFFFFFFFULL;
  // build a mask which is 0.5, i.e. an exponent equal to 1022
  // which means *2, see the above +1.
  const uint64_t p05 = 0x3FE0000000000000ULL; //dp2uint64(0.5);
  n |= p05;

  return uint642dp(n);
}

//------------------------------------------------------------------------------
/// Like frexp but vectorising and the exponent is a float.
inline float getMantExponentf(const float x, float & fe){

    uint32_t n = sp2uint32(x);
    int32_t e = (n >> 23)-127;
    fe = e;

    // fractional part
    const uint32_t p05f = 0x3f000000; // //sp2uint32(0.5);
    n &= 0x807fffff;// ~0x7f800000;
    n |= p05f;

    return uint322sp(n);

}

//------------------------------------------------------------------------------
/// Converts a fp to an int
inline uint32_t fp2uint(float x) {
    return sp2uint32(x);
  }
/// Converts a fp to an int
inline uint64_t fp2uint(double x) {
    return dp2uint64(x);
  }
/// Converts an int to fp
inline float int2fp(uint32_t i) {
    return uint322sp(i);
  }
/// Converts an int to fp
inline double int2fp(uint64_t i) {
    return uint642dp(i);
  }

//------------------------------------------------------------------------------
/**
 * A vectorisable floor implementation, not only triggered by fast-math.
 * These functions do not distinguish between -0.0 and 0.0, so are not IEC6509 
 * compliant for argument -0.0
**/ 
inline double fpfloor(const double x){
  // no problem since exp is defined between -708 and 708. Int is enough for it!
  int32_t ret = int32_t (x);
  ret-=(sp2uint32(x)>>31);  
  return ret;

}
//------------------------------------------------------------------------------
/**
 * A vectorisable floor implementation, not only triggered by fast-math.
 * These functions do not distinguish between -0.0 and 0.0, so are not IEC6509 
 * compliant for argument -0.0
**/ 
inline float fpfloor(const float x){
  int32_t ret = int32_t (x);
  ret-=(sp2uint32(x)>>31);  
  return ret;

}

//------------------------------------------------------------------------------




}

} // end of namespace vdt

#endif /* VDTCOMMON_H_ */
