/*
 * hyperbolic.h
 * The basic idea is to exploit Pade polynomials.
 * A lot of ideas were inspired by the cephes math library (by Stephen L. Moshier
 * moshier@na-net.ornl.gov) as well as actual code. 
 * The Cephes library can be found here:  http://www.netlib.org/cephes/
 * 
 *  Created on: May 15, 2017
 *      Author: Chernov Sergey
 */



#ifndef __HYPERBOLIC_H__
#define __HYPERBOLIC_H__

#include "vdtcore_common.h"
#include "exp.h"

namespace vdt
{


// Double precision sinh
inline double fast_sinh( const double x )
{
  double  exp = 0.5 * fast_exp(x);
  return  exp - 0.25/exp;
}

// Double precision sinh
inline double fast_cosh( const double x )
{
  double  exp = 0.5 * fast_exp(x);
  return  exp + 0.25/exp;
}

// Double precision tanh
inline double fast_tanh( const double x )
{
  double  exp = fast_exp(2.0*x);
  return  (exp-1.0)/(exp+1.0);
}

// Single precision sinh
inline float fast_sinhf( const float x )
{
  float  exp = 0.5f * fast_exp(x);
  return exp - 0.25f / exp;
}

// Single precision sinh
inline float fast_coshf( const float x )
{
  float  exp = 0.5f * fast_exp(x);
  return exp + 0.25f / exp;
}

// Single precision tanh
inline float fast_tanhf( const float x )
{
  float  exp = fast_exp(2.0f*x);
  return  (exp-1.0f)/(exp+1.0f);
}


} // end namespace vdt

#endif  // __HYPERBOLIC_H__
