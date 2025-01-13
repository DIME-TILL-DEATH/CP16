/*
 * kgp_fft.h
 *
 *  Created on: 14 янв. 2016 г.
 *      Author: klen
 *
 *      nonrecursive methon implement in this source as more speed
 *
 *      recursive method:
 *      http://acm.mipt.ru/twiki/bin/view/Algorithms/FastFourierTransformCPP
 */

#ifndef __FFT_H__
#define __FFT_H__

#include "vdt/vdtMath.h"
#include "math/matrix2f.h"

namespace kgp_math
{
  typedef matrix2::vector vec ;
  
  class fft
		{
		   public:

	          __attribute__((always_inline)) fft( vec* X, size_t N, vec* Wfwd, vec* Winv = 0 )
		         {
	        	    init(X, N, Wfwd, Winv);
		         }
	          __attribute__((always_inline)) ~fft()
	             {
	             }

	          __attribute__((always_inline)) fft()
		         {
		         }

	         inline void __attribute__((always_inline)) init(vec* X, size_t N, vec* Wfwd, vec* Winv = 0)
		         {
	        	   this->X = X ;
	        	   this->N = N ;
	        	   this->Wfwd = Wfwd ;
	        	   this->Winv = Winv ;

	        	   bit_shift = __builtin_clz(N)+1 ;

	        	   roots();
	        	   if (Winv)
	        	   	 roots(true);
		         }

	         inline size_t __attribute__((always_inline)) reverse_index(size_t i)
	             {
	        	    return  __builtin_rbit(i) >> bit_shift ;
	             }

	         // on-fly reposition form write
	         inline void __attribute__((always_inline)) reposition_write( vec& val , size_t n )
	             {
	        	    X[ reverse_index(n) ] = val ;
	             }

	         inline void __attribute__((always_inline)) reposition()
	             {
	        	   // Swap elements at positions 'i' and reverse('i')
	        	   for(size_t i = 0; i < N; i++)
	        	     {
	        	       // bit 'i' reverse and align for 'j' index calculate
	        	       size_t j = reverse_index(i);
	        	       if(i <= j)
	        		 {
	        	           vec tmp = X[i] ;
	        	           X[i] = X[j] ;
	        	           X[j] = tmp ;
	        		 }
	        	   }
	             }

	         inline void __attribute__((always_inline)) roots( bool inverse = false)
	             {
	                vec*& W = inverse ? Winv : Wfwd ;
	        	    size_t bits = 32 - __builtin_clz(N) - 1 ;
	                for (size_t i = 0; i < bits; i++)
	         	       {
	         		  size_t m = 1 << i;
	         		  size_t n = m * 2;
	         	          float alpha = inverse ? vdt::details::VDT_2PI_F / n : -vdt::details::VDT_2PI_F / n ;
	         	          float s,c ;
	         	          vdt::fast_sincosf(alpha,s,c);
	         	          W[i].x=c;
	         	          W[i].y=s;
	         	       }
	             }

                inline void __attribute__((always_inline)) batterfly(vec* W )
                    {
            	       size_t bits = 32 - __builtin_clz(N) - 1 ;
            	       for (size_t i = 0; i < bits; i++)
            	         {
            		    size_t m = 1 << i;
            		    size_t n = m * 2;

            	            vec Wik(1,0) ; // init val of e^(-2*pi/N*k)

            	            for (size_t k = 0; k < m; k++)
            	              {

            	                 for (size_t j = k; j < N; j += n)
            	                   {
            	                     vec x = X[j];
            	                     vec y = Wik * X[j + m];
            	                     X[j]     = x + y;
            	                     X[j + m] = x - y;
            	                   }

            	                 Wik *= W[i] ; // rotate W by Wi
            	             }
            	        }
                   }

             inline void __attribute__((always_inline)) batterfly()
                {
            	   batterfly(Wfwd);
                }

             inline void __attribute__((always_inline)) batterfly_inverse()
                {
            	   batterfly(Winv);
                }

             inline void __attribute__((always_inline)) transform()
                {
            	   reposition() ;
            	   batterfly (Wfwd);
                }

             inline void __attribute__((always_inline)) transform_inverse()
                {
            	   batterfly (Winv);
            	   reposition() ;
            	   for (size_t n = 0; n < N; n++ )
            		   X[n] /= N ;
                }

             inline size_t __attribute__((always_inline)) getN() {return N; };
             inline vec*   __attribute__((always_inline)) ptrX() {return X; };
             inline vec*   __attribute__((always_inline)) ptrWfwd() {return Wfwd; };
             inline vec*   __attribute__((always_inline)) ptrWinv() {return Winv; };
             inline vec&   __attribute__((always_inline)) operator[] (size_t i)
                {
            	    return X[i] ;
                }



             // utility
             inline size_t __attribute__((always_inline)) find_max()
                {
            	  return std::max_element(X, X+N/2, matrix2::compare) - X ;
                }

             inline float __attribute__((always_inline)) k2freq(float k, float Fs)
		{
            	  return k * Fs / N ;
		}

             inline void __attribute__((always_inline)) log()
                {
                    for (size_t n = 0 ; n < N ; n++)
                 	X[n].logiate() ;
                }

             inline void __attribute__((always_inline)) log(vec* out)
                {
                    for (size_t n = 0 ; n < N ; n++)
                        out[n] = X[n].log() ;
                }

             inline void __attribute__((always_inline)) dBv(float ref_value, float* out)
                {
                    for (size_t n = 0 ; n < N ; n++)
                        out[n] = 20.0f*vdt::fast_logf(X[n].abs()/ref_value) ;
                }

             inline void __attribute__((always_inline)) dBw(float ref_power, float* out)
                {
                    for (size_t n = 0 ; n < N ; n++)
                        out[n] = 10.0f* vdt::fast_logf(X[n].power()/ref_power) ;
                }

    protected:

    private:
              vec* X ;
              size_t N  ;
              vec* Wfwd ;
              vec* Winv ;

              // utility vars
              size_t bit_shift ;
		};

  void inline __attribute__((always_inline))  multiply(fft& fft_0, fft& fft_1, fft& fft_prod)
  {
     for(size_t i = 0; i < fft_0.getN(); i++)
       fft_prod[i] = fft_0[i] * fft_1[i];
  }

  /*
   * Finds the convolution of two vectors (the product of two polynomials, given
   * that the result has power less than ``N'').  ``N'' must be a power of
   * 2.
  */
  inline fft& __attribute__((always_inline)) convolution(fft& fft_0, fft& fft_1)
   {
     //// TODO
     fft_0.transform();
     fft_1.transform();
     multiply(fft_0, fft_1, fft_0 );
     fft_0.transform_inverse();
     return fft_0 ;
   }

   inline float __attribute__((always_inline)) wrap(float angle, float period)
         {
             auto qpd = (int) (angle/period);

             if (qpd >= 0)
            	 qpd += qpd & 1;
             else
            	 qpd -= qpd & 1;

             angle -= period*qpd;
             return angle;
         }

   inline float __attribute__((always_inline)) wrap_2_pi(float angle)
         {
             auto qpd = (int) (angle * vdt::details::VDT_1_2PI);

             if (qpd >= 0)
            	 qpd += qpd & 1;
             else
            	 qpd -= qpd & 1;

             angle -= vdt::details::VDT_2PI_F * qpd;
             return angle;
         }

   inline float __attribute__(/*(always_inline)*/(optimize(0))) find_tone( float& f_mesured,  fft& fft_0, fft& fft_1, size_t n)
   {

      // вычисление номера частотного отсчета с максимальной мощностью
      size_t k = fft_0.find_max();

      // приведенное время перекрытия
      auto dt = 1.0f * n / fft_0.getN() ;

      // угловая частота k-ого отсчета спектра
      auto omega_expected = k * vdt::details::VDT_2PI_F ;

      //
      auto omega_actual = (fft_1[k].arg() - fft_0[k].arg()) / dt; // ω=∂φ/∂t


      auto omega_delta = wrap_2_pi(omega_actual - omega_expected); // Δω=(∂ω + π)%2π - π

      auto binDelta = omega_delta * vdt::details::VDT_1_2PI ;

      f_mesured = k + binDelta ;

  	  return  (fft_1[k].abs() + fft_0[k].abs())*(0.5f + vabs(binDelta));

   }

};

using namespace kgp_math ;

#endif /* __FFT_H__ */
