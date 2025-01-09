

#ifndef POW_H_
#define POW_H_

#include "vdtcore_common.h"

#include "log.h"
#include "exp.h"

namespace vdt{

// local namespace for the constants/functions which are necessary only here
namespace details
{
}

// pow double precision --------------------------------------------------------
inline double fast_pow(const double base , const double exp)
{
  if ( base >= 0.0 )
    return fast_exp(exp * vdt::fast_log(base) ) ;
  else
    {
      //__throw_domain_error("negative 'base' value");
      return std::numeric_limits<double>::quiet_NaN() ;
    }
}

// Log single precision --------------------------------------------------------



namespace details
{
}

// Log single precision --------------------------------------------------------
inline float fast_powf(const float base , const float exp)
{
  if ( base >= 0.0f )
    return fast_expf(exp * vdt::fast_logf(base) ) ;
  else
    {
      //__throw_domain_error("negative 'base' value");
      return std::numeric_limits<float>::quiet_NaN() ;
    }
}


} //vdt namespace

#endif /* POW_H_ */
