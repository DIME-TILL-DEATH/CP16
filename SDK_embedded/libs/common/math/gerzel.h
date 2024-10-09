/*
 * gerzel.h
 *
 *  Created on: 26 марта 2014 г.
 *      Author: klen
 */

#ifndef GERZEL_H_
#define GERZEL_H_


class TGerzel
{
  public:
    TGerzel(size_t N, size_t K)
      {
        this->N = N ;
        this->K = K ;
        Xn  = new signal_t [N] ;
        Wk  = new signal_t [K] ;
        Skn = new signal_t [K] ;

        for ( size_t k = 0 ; k < K ; k++  )
          {
            Wk[k] = signal_t( cosf(2.0f*M_PI*k/N) , -sinf(2.0f*M_PI*k/N) );
          }

        n = 0 ;
      }
    ~TGerzel()
      {
        delete [] Xn ;
        delete [] Wk ;
        delete [] Skn ;
      }

    size_t N ;
    size_t K ;
    signal_t* Skn ; // спектральный отсчет
    signal_t* Wk ; // фазовращающий множитель
    signal_t* Xn ;
    size_t n ;

    inline void Update( signal_t xn )
    {
      for ( size_t k = 0 ; k < K ; k++  )
            Skn[k] = Wk[k] * ( Skn[k] + xn - Xn[n] ) ;

      Xn[n++] = xn ;
      if ( n == N ) n = 0 ;
    }

    inline float Power( size_t k ) { return  Skn[k].x*Skn[k].x + Skn[k].y*Skn[k].y; }
    inline float Amp( size_t k ) { return  vsqrt(Power(k)); }
} ;


#endif /* GERZEL_H_ */
