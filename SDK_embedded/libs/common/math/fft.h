/*
 * fft.h
 *
 *  Created on: 11 янв. 2016 г.
 *      Author: klen
 *
 *      http://acm.mipt.ru/twiki/bin/view/Algorithms/FastFourierTransformCPP
 *
 *      klen_s@mail.ru
 *      ported to std container style
 */



#ifndef __FFT_H__
#define __FFT_H__

/*
 * Fast Fourier Transform (implementation)
 *
 * Daniel Shved, MIPT, 2010.
 * danshved [at] gmail.com
 */

#include "vdt/vdtMath.h"
#include "math/matrix2f.h"

#include <algorithm>
typedef matrix2::vector vec ;
typedef std::vector<vec>     vec_vector ;

using namespace std;

/*
 * "Butterfly" transform.
 */
inline void butterfly(vec &x, vec &y, const vec& w)
{
  vec r = y*w;
  y = x - r;
  x = x + r ;
}

// Series of butterfly transforms required by the FFT algorithm.
inline void mass_butterfly(vec *array, const size_t N, const vec& root)
{
   vec power(1.0, 0.0);
   size_t n = N/2;

   for(size_t i = 0; i < n; i++)
     {
      butterfly(array[i], array[i+n], power);
      power *= root;
     }
}


// index for 'on fly' write data to input buffer in reposition FFT order
inline size_t reposition_index(size_t i, size_t N)
{
  return __builtin_rbit(i) >> (__builtin_clz(N)+1) ;
}

/*
 * Moves elements of the array as required by the iterative FFT implementation.
 * ``N'' must be a power of 2.
 */
inline void reposition(vec *array, size_t N)
{
   size_t bit_shift = __builtin_clz(N)+1 ;

   // Swap elements at positions 'i' and reverse('i')
   for(size_t i = 0; i < N; i++)
     {
       // bit 'i' reverse and align for 'j' index calculate
       size_t j = __builtin_rbit(i) >> bit_shift ;
       if(i <= j)
         swap(array[i], array[j]);
   }
}

inline void fft_roots(vec_vector& roots, const size_t N, bool inverse)
{
     float c,s ;
     vdt::fast_sincosf(vdt::details::VDT_2PI_F/N,s,c);
     vec root(c, inverse ? -s : s );
     for(size_t step=N; step != 1; step /= 2)
       {
          roots.insert(roots.begin(),root);
          root = root*root;
       }
}

/*
 * Does the Discrete Fourier Transform.  Takes time O(N * log(N)).
 * ``N'' must be a power of 2.
 */
void inline fft(vec *array, vec_vector& roots, const size_t N)
{
   // Arrange numbers in a convenient order
   reposition(array, N);

   // Prepare roots of unity for every step
   size_t step;

   vec_vector::iterator it = roots.begin();
   vec root ;
   // Do lots of butterfly transforms
   for(step = 2; step <= N; step *= 2)
     {
        root = *it++ ;
        for(size_t i = 0; i < N; i += step)
	    mass_butterfly(array + i, step, root);
     }
}

/*
 * Replaces every element of the vector by its complex conjugate.
 */
void inline conjugate(vec *array, const size_t N)
{
   for(size_t i = 0; i < N; i++)
     array[i].conjze();
}

/*
 * The inverse DFT.
 * use direct phase rotater W array prodused by fourier_roots(..., ..., false)
 */
void inline ifft(vec *array, vec_vector& roots, const size_t N, bool use_direct_roots = false)
{
   if (use_direct_roots)
      conjugate(array, N);

   fft(array, roots, N);

   if (use_direct_roots)
      conjugate(array, N);

   for(size_t i = 0; i < N; i++)
      array[i] /= N;
}

/*
 * Multiplies two vectors element by element.
*/
void inline multiply(vec *arr1, vec *arr2, vec *result, const size_t N)
{
   for(size_t i = 0; i < N; i++)
      result[i] = arr1[i] * arr2[i];
}


/*
 * Finds the convolution of two vectors (the product of two polynomials, given
 * that the result has power less than ``N'').  ``N'' must be a power of
 * 2.
*/
void inline convolution(vec *arr1, vec *arr2, vec_vector& roots ,vec *result, const size_t N)
{
   fft(arr1, roots, N);
   fft(arr2, roots, N);
   multiply(arr1, arr2, result, N);
   ifft(result, roots, N, true);
}

#endif /* __FFT_H__ */
