/*
 * pitch_filter.h
 *
 *  Created on: 27 марта 2014 г.
 *      Author: klen
 */

#ifndef PITCH_FILTER_H_
#define PITCH_FILTER_H_

//#include "math/matrix2f.h"
//#include "sincos.h"

#include "vdt/sincos.h"
#include "vdt/atan2.h"
#include "math/matrix2f.h"

typedef matrix2::vector signal_t ;

class gertzel_t
{
  public:

    typedef matrix2::vector signal_t ;

    gertzel_t(const size_t N, const size_t K): N(N), K(K)
      {

        Xin_n  = new signal_t [N] ;
        W_k    = new signal_t [K] ;
        S_kn   = new signal_t [K] ;


        for ( size_t k = 0 ; k < K ; k++  )
          {
            vdt::fast_sincosf(2.0f*vdt::details::M_PI*k/N , W_k[k].sin , W_k[k].cos ) ;
            W_k[k].sin*=-1 ;

            S_kn[k]= signal_t(0.0f, 0.0f) ;
          }

        n = 0 ;
      }
    ~gertzel_t()
      {

        delete [] Xin_n ;
        delete [] W_k ;
        delete [] S_kn ;

      }

    const size_t N ;
    const size_t K ;       // количество фильтров
    signal_t* S_kn ; // спектральный отсчет
    signal_t* W_k ; // фазовращающий множитель
    signal_t* Xin_n ;
    size_t n ;

    //float* Fi_k ; // фаза гармоники синтеза
    //float Shift ; // сдвиг спектра [0,6...1.4]

    inline float update( const signal_t& xin_n )
    {

      float xout_n = 0 ;

      for ( size_t k = 0 ; k < K ; k++  )
        {
          // спектральный анализ потока
          S_kn[k] = W_k[k] * ( S_kn[k] + xin_n - Xin_n[n] ) ;
/*
          // спектральный синтез сигнала
          size_t k_out = k * Shift ; // смещенный индекс частной составляющей
          float amp = 0.0f ;
          if ( k_out < K ) amp = Amp(k * Shift) ;
          xout_n += amp * vsin( Fi_k[k] ) ;

          Fi_k[k] += 2*M_PI/N * k ;
*/
        }

 //     xout_n /= N ;

      Xin_n[n++] = xin_n ;
      if ( n == N ) n = 0 ;

      return xout_n ;

    }

    inline float power( const size_t k ) const  { return  S_kn[k].x*S_kn[k].x + S_kn[k].y*S_kn[k].y; }
    inline float amp  ( const size_t k ) const  { return  vsqrt(power(k)); }
    inline float arg  ( const size_t k ) const  { return  vdt::fast_atan2f( S_kn[k].y , S_kn[k].x ); }

} ;

#endif /* PITCH_FILTER_H_ */
