/*
 * matrix3f.h
 * target a cortex-m4f (using vsqrt instr)
 *
 *  Created on: 06.07.2012
 *      Author: klen
 */

#ifndef __MATRIX3F_H__
#define __MATRIX3F_H__

#include <math.h>
#include <stddef.h>

// support hardware sqrt abs function if available (cortex-m4f)

#include "arch.h"

namespace matrix3
{

const float machine_epsilon = 1.1920928955078125e-07;

//class vector ;
class matrix ;
class quat ;

inline float machine_eps()
{
	float val = 1.0f;
    	do
	{
		 val /= 2.0f;
	}
    	while ((1.0f + val/2.0f) != 1.0f);
	return val ;
	 
}


// vector class --------------------------------------------------
class vector
{

	friend class matrix ;
	friend class quat ;
	private:

	public :

	union
	{
           struct { float x, y, z ; } __PACKED__ ;
           float p[3] ;
	} __PACKED__ ;

			inline __attribute__((always_inline)) vector (): x(0),y(0),z(0) {}

			// set constructor
			inline __attribute__((always_inline)) vector( const float& x , const float& y , const float& z )
			{
				this->x = x; this->y = y; this->z = z;
			}
			// vectors set constructor
			inline __attribute__((always_inline)) vector( const vector& v )
			{
				x = v.x; y = v.y; z = v.z;
			}

			   // operators
			   inline vector& __attribute__((always_inline)) operator = (const vector& src)
			   	   {
				   	   x = src.x; y = src.y; z = src.z;
				   	   return *this ;
			   	   }


			   // Unary operators
			   vector __attribute__((always_inline)) operator + ()  { return *this; }
			   vector __attribute__((always_inline)) operator - ()
			   	   	   {
				   	   	   vector tmp ;
				   	   	   tmp.x = -this->x ;
				   	   	   tmp.y = -this->y ;
				   	   	   tmp.z = -this->z ;
				   	   	   return tmp ;

			   	   	   }

			   // Combined assignment - calculation operators
			   vector& __attribute__((always_inline)) operator += (const vector& src)
					{
					   x += src.x; y += src.y; z += src.z;
					   return *this ;
					}

	                   vector& __attribute__((always_inline)) operator += (const float& src)
	                                {
	                                   x += src; y += src; z += src;
	                                   return *this ;
	                                }

			   vector& __attribute__((always_inline)) operator -= (const vector& src)
					{
				   	   x -= src.x; y -= src.y; z -= src.z;
				   	   return *this ;
					}
			   vector& __attribute__((always_inline)) operator -= (const float& src)
			                {
			                   x -= src; y -= src; z -= src;
			                   return *this ;
			                }

			   vector& __attribute__((always_inline)) operator *= (const float& c)
					{
			   	   	   x *= c; y *= c; z *= c;
			   	   	   return *this ;
					}

			   vector& __attribute__((always_inline)) operator *= (const vector& v)
					{
			   	   	   vector tmp(*this);
					   x = tmp.y*v.z-tmp.z*v.y ;
					   y = tmp.z*v.x-tmp.x*v.z ;
					   z = tmp.x*v.y-tmp.y*v.x ;	
			   	   	   return *this ;
					}

			   vector& __attribute__((always_inline)) operator /= (const float& c)
					{
			   	   	   *this *= 1.0f/c ;
			   	   	   return *this ;
					}

			   // Miscellaneous -methods
			   inline void __attribute__((always_inline)) null () { x = y = z = 0.0f; } ;
			   inline void __attribute__((always_inline)) identity () { x = y = z = 1.0f; } ;

			   inline float __attribute__((always_inline)) norma1 () {  return vabs(x) + vabs(y) + vabs(z) ; }
			   inline vector& __attribute__((always_inline)) normalize1 () {  (*this) *= 1.0f/norma1 (); return *this;}
			   inline float __attribute__((always_inline)) norma () {  return vsqrt( x*x + y*y +z*z ) ; }
			   inline vector& __attribute__((always_inline)) normalize () {  (*this) *= 1.0f/norma (); return *this; }
			   inline vector __attribute__((always_inline)) ort () {  vector tmp(*this); return tmp.normalize(); }

};

// logical equal-to operator
inline bool __attribute__((always_inline))
operator == (const vector& a, const vector& b)
{
   return (a.x == b.x) &&  (a.y == b.y) && (a.z == b.z) ;
}

// logical no-equal-to operator
inline bool __attribute__((always_inline))
operator != (const vector& v1, const vector& v2)
{
    return !(v1 == v2) ;
}

// binary addition operator
inline vector __attribute__((always_inline))
operator + (const vector& v1, const vector& v2)
{
   vector temp = v1;
   temp += v2;
   return temp;
}

// binary addition operator
inline vector __attribute__((always_inline))
operator + (const vector& v1, const float& c)
{
   vector temp = v1;
   temp += c;
   return temp;
}

// binary subtraction operator
inline vector __attribute__((always_inline))
operator - (const vector& v1, const vector& v2)
{
   vector temp(v1);
   temp -= v2;
   return temp;
}

// binary subtraction operator
inline vector __attribute__((always_inline))
operator - (const vector& v1, const float& c)
{
   vector temp(v1);
   temp -= c;
   return temp;
}

// binary scalar multiplication operator
inline vector __attribute__((always_inline))
operator * (const vector& v, const float& c)
{
   vector tmp(v);
   return tmp*=c;
}

// binary scalar multiplication operator
inline vector __attribute__((always_inline))
operator * (const float& c, const vector& v)
{
	vector tmp(v);   
	return tmp*=c ;
}

inline vector __attribute__((always_inline))
operator * (const vector& v0, const vector& v1)
{
  vector tmp(v0); 
  return tmp*=v1 ;
}

// binary scalar division operator
inline vector __attribute__((always_inline))
operator / (const vector& v, const float& c)
{
	vector tmp(v) ;   
	return tmp/=c ;
}

inline  float __attribute__((always_inline)) 
operator ^ (vector& v0 , vector& v1 )
{
 return v0.x*v1.x + v0.y*v1.y + v0.z*v1.z ;
} 

// matrix class --------------------------------------------------
class matrix
{
	friend class vector ;
	friend class quat ;
	private:


	public:

		union
		{
	        struct { float 	m00, m01, m02,
	                        m10, m11, m12,
	                        m20, m21, m22 ; } __PACKED__ ;
	        float p[9] ;

		} __PACKED__ ;

		// zero fill constructor
		inline __attribute__((always_inline)) matrix()	{}
		// set constructor
		inline __attribute__((always_inline)) matrix( 	float m00 , float m01 , float m02 ,
						float m10 , float m11 , float m12 ,
						float m20 , float m21 , float m22 )
		{
			this->m00 = m00; this->m01 = m01; this->m02 = m02;
			this->m10 = m10; this->m11 = m11; this->m12 = m12;
			this->m20 = m20; this->m21 = m21; this->m22 = m22;
		}
		// vectors set constructor
		inline __attribute__((always_inline)) matrix( vector& x0 , vector& y0, vector& z0 )
		{
			m00 = x0.x; m01 = x0.y; m02 = x0.z;
			m10 = y0.x; m11 = y0.y; m12 = y0.z;
			m20 = z0.x; m21 = z0.y; m22 = z0.z;
		}

		// matrix set constructor
		inline __attribute__((always_inline)) matrix( const matrix& m )
		{
			m00 = m.m00; m01 = m.m01; m02 = m.m02;
			m10 = m.m10; m11 = m.m11; m12 = m.m12;
			m20 = m.m20; m21 = m.m21; m22 = m.m22;
		}


		   // operators
		   inline matrix& __attribute__((always_inline)) operator = (const matrix& m)
		   	   {
			   	   m00 = m.m00; m01 = m.m01; m02 = m.m02;
			   	   m10 = m.m10; m11 = m.m11; m12 = m.m12;
			   	   m20 = m.m20; m21 = m.m21; m22 = m.m22;
			   	   return *this ;
		   	   }


		   // Unary operators
		   inline matrix __attribute__((always_inline)) operator + () const  { return *this; }
		   inline matrix __attribute__((always_inline)) operator - () const
		   	   	   {
			   	   	   matrix tmp(  -this->m00 , -this->m01 , -this->m02 ,
			   	   		        -this->m10 , -this->m11 , -this->m12 ,
			   	   		        -this->m20 , -this->m21 , -this->m22
			   	   	   	      ) ;
			   	   	   return tmp ;

		   	   	   }

		   // Combined assignment - calculation operators
		   inline matrix& __attribute__((always_inline)) operator += (const matrix& src)
				{
				   m00 += src.m00; m01 += src.m01; m02 += src.m02;
				   m10 += src.m10; m11 += src.m11; m12 += src.m12;
				   m20 += src.m20; m21 += src.m21; m22 += src.m22;
				   return *this ;
				}
		   inline matrix& __attribute__((always_inline)) operator -= (const matrix& m)
				{
			   	   m00 -= m.m00; m01 -= m.m01; m02 -= m.m02;
			   	   m10 -= m.m10; m11 -= m.m11; m12 -= m.m12;
			   	   m20 -= m.m20; m21 -= m.m21; m22 -= m.m22;
			   	   return *this ;
				}
		   inline matrix& __attribute__((always_inline)) operator *= (const matrix& m)
				{
			   	   matrix tmp(*this) ;

			   	   m00 = tmp.m00*m.m00 + tmp.m01*m.m10 + tmp.m02*m.m20 ;  m01 = tmp.m00*m.m01 + tmp.m01*m.m11 + tmp.m02*m.m21 ; m02 = tmp.m00*m.m02 + tmp.m01*m.m12 + tmp.m02*m.m22  ;
			   	   m10 = tmp.m10*m.m00 + tmp.m11*m.m10 + tmp.m12*m.m20 ;  m11 = tmp.m10*m.m01 + tmp.m11*m.m11 + tmp.m12*m.m21 ; m12 = tmp.m10*m.m02 + tmp.m11*m.m12 + tmp.m12*m.m22  ;
			   	   m20 = tmp.m20*m.m00 + tmp.m21*m.m10 + tmp.m22*m.m20 ;  m21 = tmp.m20*m.m01 + tmp.m21*m.m11 + tmp.m22*m.m21 ; m22 = tmp.m20*m.m02 + tmp.m21*m.m12 + tmp.m22*m.m22  ;

			   	   return *this ;
				}
		   inline matrix& __attribute__((always_inline)) operator *= (const float& c)
				{
		   	   	   m00 *= c; m01 *= c; m02 *= c;
		   	   	   m10 *= c; m11 *= c; m12 *= c;
		   	   	   m20 *= c; m21 *= c; m22 *= c;
		   	   	   return *this ;
				}
		   inline matrix& __attribute__((always_inline)) operator /= (const float& c)
				{
		   	   	   float c_inv = 1.0f/c ;
		                   m00 *= c_inv; m01 *= c_inv; m02 *= c_inv;
		   	   	   m10 *= c_inv; m11 *= c_inv; m12 *= c_inv;
		   	   	   m20 *= c_inv; m21 *= c_inv; m22 *= c_inv;
		   	   	   return *this ;
				}

		   inline float __attribute__((always_inline)) operator ()( const size_t a , const size_t b ) const
		   {
		     return p[ 3*a + b ] ;
		   }
	           inline float& __attribute__((always_inline)) operator ()( const size_t a , const size_t b )
	           {
	             return p[ 3*a + b ] ;
	           }

		   // Miscellaneous -methods
		   inline void __attribute__((always_inline)) null () { m00 = m01 = m02 = m10 = m11 = m12 = m20 = m21 = m22 = 0.0f; } ;
		   inline void __attribute__((always_inline)) one ()  { m00 = m01 = m02 = m10 = m11 = m12 = m20 = m21 = m22 = 1.0f; } ;
	           inline void __attribute__((always_inline)) identity ()  { m01 = m02 = m10 = m12 = m20 = m21=0.0f ; m00 = m11= m22 = 1.0f;} ;

		   // Utility methods
		   inline matrix __attribute__((always_inline)) solve (const matrix& b) const ;
		   inline vector __attribute__((always_inline)) solve (const vector& b) const ;

		   inline matrix __attribute__((always_inline)) adj () const { return matrix() ; }
		   inline float __attribute__((always_inline)) trace() const { return m00 + m11 + m22 ; }
		   inline __attribute__((always_inline)) matrix& inv ()
		   	   {
					matrix tmp(*this) ;
					float d_inv = 1.0f/det() ;
					m00 =  ( tmp.m11*tmp.m22 - tmp.m21*tmp.m12 ) ;     m10 =  -( tmp.m10*tmp.m22 - tmp.m20*tmp.m12 ) ;  m20 =  ( tmp.m10*tmp.m21 - tmp.m20*tmp.m11 );
					m01 = -( tmp.m01*tmp.m22 - tmp.m21*tmp.m02 ) ;     m11 =   ( tmp.m00*tmp.m22 - tmp.m20*tmp.m02 ) ;  m21 = -( tmp.m00*tmp.m21 - tmp.m20*tmp.m01 );
					m02 =  ( tmp.m01*tmp.m12 - tmp.m11*tmp.m02 ) ;     m12 =  -( tmp.m00*tmp.m12 - tmp.m10*tmp.m02 ) ;  m22 =  ( tmp.m00*tmp.m11 - tmp.m10*tmp.m01 );
					(*this) *= d_inv ;
					return *this ;
		   	   }
		   inline float __attribute__((always_inline)) det () const
		   	   {
			   	   return m00*( m11*m22 - m21*m12 ) - m01*(m10*m22 - m20*m12) + m02*(m10*m21 - m20*m11) ;
		   	   }

		   inline matrix& __attribute__((always_inline))  transpose ()
			{
				matrix tmp ( 	m00 , m10, m20,
						m01 , m11, m21,
						m02 , m12, m22) ;
				*this=tmp ;
				return *this ;
			}

		   inline float __attribute__((always_inline))  norma1 () const {  return vabs(m00) + vabs(m01) + vabs(m02) +
		                                    vabs(m10) + vabs(m11) + vabs(m12) +
		                                    vabs(m20) + vabs(m21) + vabs(m22) ; }
		   inline matrix& __attribute__((always_inline)) normalize1 () {  *this *= 1.0f/norma1 () ; return *this;  }

		   inline float __attribute__((always_inline)) norma () const {  return vsqrt(     m00*m00 + m01*m01 + m02*m02 +
							      m10*m10 + m11*m11 + m12*m12 +
							      m20*m20 + m21*m21 + m22*m22 ) ; }
		   inline matrix& __attribute__((always_inline)) normalize () {  *this *= 1.0f/norma () ; return *this; }

		   inline float __attribute__((always_inline)) cofact () const {return 0.0f ;}
		   inline float __attribute__((always_inline)) cond () const {return 0.0f ;}

};


// unary inversion
inline matrix __attribute__((always_inline)) operator ! (const matrix& m)
		{
			matrix tmp ( m ) ;
			return tmp.inv()  ;
		}

// logical equal-to operator
inline bool __attribute__((always_inline))
operator == (const matrix& a, const matrix& b)
{
   return (a.m00 == b.m00) &&  (a.m01 == b.m01) && (a.m02 == b.m02) &&
	  (a.m10 == b.m10) &&  (a.m11 == b.m11) && (a.m12 == b.m12) &&
	  (a.m20 == b.m20) &&  (a.m21 == b.m21) && (a.m22 == b.m22) ;
}

// logical no-equal-to operator
inline bool __attribute__((always_inline))
operator != (const matrix& a, const matrix& b)
{
    return !(a == b) ;
}

// binary addition operator
inline matrix __attribute__((always_inline))
operator + (const matrix& m1, const matrix& m2)
{
   matrix temp = m1;
   temp += m2;
   return temp;
}

// binary subtraction operator
inline matrix __attribute__((always_inline))
operator - (const matrix& m1, const matrix& m2)
{
   matrix temp(m1);
   temp -= m2;
   return temp;
}


// binary scalar multiplication operator
inline vector __attribute__((always_inline))
operator * (const matrix& m, const vector& c)
{
   vector tmp;
   tmp.x = m.m00 * c.x + m.m01 * c.y + m.m02 * c.z ;
   tmp.y = m.m10 * c.x + m.m11 * c.y + m.m12 * c.z ;
   tmp.z = m.m20 * c.x + m.m21 * c.y + m.m22 * c.z ;
   return tmp;
}


// binary scalar multiplication operator
inline matrix __attribute__((always_inline))
operator * (const matrix& m, const float& c)
{
   matrix temp(m);
   temp *= c;
   return temp;
}

// binary scalar multiplication operator
inline matrix __attribute__((always_inline))
operator * (const float& c, const matrix& m)
{
   return (m * c);
}

// binary matrix multiplication operator
inline matrix __attribute__((always_inline))
operator * (const matrix& m1, const matrix& m2)
{
   matrix temp = m1;
   temp *= m2;
   return temp;
}

// binary scalar division operator
inline matrix __attribute__((always_inline))
operator / (const matrix& m, const float& c)
{
    return (m * (1.0f / c));
}

// binary scalar division operator
inline matrix __attribute__((always_inline))
operator / (const float& no, const matrix& m)
{
    return (!m * no);
}

// binary matrix division operator
inline matrix __attribute__((always_inline))
operator / (const matrix& m1, const matrix& m2)
{
    return (m1 * !m2);
}



// unary transpose --------------------------------------------
inline matrix __attribute__((always_inline))  operator ~ (const matrix& src)
	{
		matrix tmp ( 	src.m00 , src.m10, src.m20,
				src.m01 , src.m11, src.m21,
				src.m02 , src.m12, src.m22) ;
		return tmp ;
	}

/*
// binary power operator
inline matrix __attribute__((always_inline))
operator ^ (const matrix& m, const unsigned int& pow)
{
   matrix temp = m;
   temp ^= pow;
   return temp;
}
inline matrix& __attribute__((always_inline)) matrix::operator ???= (const unsigned int& pow)
	{
		matrix temp(*this);
		for (unsigned int i=2; i <= pow; i++)
			*this = *this * temp;
		return *this;
	}
*/


// quaternion class --------------------------------------------------

class quat
{
        friend class matrix ;
        friend class vector ;


        public:


        union
        {
          struct { float x; float y; float z; float w ; } ;
          float q[4] ;
        };


          inline __attribute__((always_inline)) quat() {}
          inline __attribute__((always_inline)) quat( const vector& v , const float f  ) { set_axis(v,f);  }
          inline __attribute__((always_inline)) quat( const float* vals  ) { q[0]=vals[0]; q[1]=vals[1]; q[2]=vals[2]; q[3]=vals[3];  }
          inline __attribute__((always_inline)) quat( const float x , const float y , const float z , const float w ) { this->x=x; this->y=y; this->z=z; this->w=w; }
          inline __attribute__((always_inline)) quat( const matrix& m ) {  set(m);  }


               // operators
               inline __attribute__((always_inline)) quat& operator = (const quat& src)
                       {
                               x = src.x; y = src.y; z = src.z; w = src.w;
                               return *this ;
                       }


               // Unary operators
               inline quat __attribute__((always_inline)) operator + () const { return quat(x,y,z,w); }
               inline quat __attribute__((always_inline)) operator - () const { return quat(-x,-y,-z,-w); }


               // Combined assignment - calculation operators
               inline quat& __attribute__((always_inline)) operator += (const quat& src)
                            {
                               x += src.x; y += src.y; z += src.z; w += src.w;
                               return *this ;
                            }

               inline quat& __attribute__((always_inline)) operator += (const float& src)
                            {
                               x += src; y += src; z += src; w += src;
                               return *this ;
                            }

               inline quat& __attribute__((always_inline)) operator -= (const quat& src)
                            {
                               *this += -src ;
                               return *this ;
                            }
               inline quat& __attribute__((always_inline)) operator -= (const float& src)
                            {
                               *this += -src ;
                               return *this ;
                            }

               inline quat& __attribute__((always_inline)) operator *= (const quat& q)
                            {
				quat tmp(*this);
				w = tmp.w * q.w - tmp.x * q.x - tmp.y * q.y - tmp.z * q.z;
        			x = tmp.w * q.x + tmp.x * q.w + tmp.y * q.z - tmp.z * q.y;
        			y = tmp.w * q.y + tmp.y * q.w + tmp.z * q.x - tmp.x * q.z;
        			z = tmp.w * q.z + tmp.z * q.w + tmp.x * q.y - tmp.y * q.x;
	                       return *this ;
                            }

               inline quat& __attribute__((always_inline)) operator *= (const float& src)
                            {
                               x *= src; y *= src; z *= src; w *= src;
                               return *this ;
                            }

               inline vector __attribute__((always_inline)) operator *= (const vector& v)
                                           {
                                              return get()*v ;
                                           }

               inline quat& __attribute__((always_inline)) operator /= (const float& c)
                            {
                               *this *= 1.0f/c ;
                               return *this ;
                            }





               // Miscellaneous -methods
               inline void __attribute__((always_inline)) null () { x = y = z = w = 0.0f; } ;
               inline void __attribute__((always_inline)) identity () { x = y = z = 0.0f; w = 1.0f; } ;

               inline float __attribute__((always_inline)) norma1() const { return vabs(x)+vabs(y)+vabs(z)+vabs(w); }
               inline quat& __attribute__((always_inline)) normalize1() { *this *= 1.0f/norma1(); return *this;}
               inline float __attribute__((always_inline)) norma() const { return vsqrt((x*x)+(y*y)+(z*z)+(w*w)); }
	       inline quat& __attribute__((always_inline)) normalize() { *this *= 1.0f/norma(); return *this; }
	       inline float __attribute__((always_inline)) norma2() const { return (x*x)+(y*y)+(z*z)+(w*w); }
	       inline quat& __attribute__((always_inline)) normalize2()  {  *this *= 1.0f/norma2(); return *this; }
	      
	       inline quat& __attribute__((always_inline)) conj() { x*=-1.0f ; y*=-1.0f ; z*=-1.0f ; return *this; }
	       inline quat& __attribute__((always_inline)) inv() 
			{  
				conj() ;
				normalize2() ; 
				return *this; 
			}


		inline void __attribute__((always_inline))  get_axis( vector& v, float& f )
                    {
                        float vl = vsqrt( x*x + y*y + z*z );
        		if( vl > machine_epsilon )
        			{
           			 float ivl = 1.0f/vl;
            			 v.x=x*ivl;
				 v.y=y*ivl;
				 v.z=z*ivl;
           			 if( w < 0 )
                			f = 2.0f*(float)atan2f(-vl, -w); //-PI,0 
            			 else
                			f = 2.0f*(float)atan2f( vl,  w); //0,PI 
        		}
			else
			{
            			v = vector(0,0,0);
            			f = 0;
       			 }
                    }

		inline quat& __attribute__((always_inline))  set_axisn( const vector& v, const float& f )
		    {
			vector tmp(v) ; 
			tmp.normalize();
			return set_axis(tmp,f);
		     }
		inline quat& __attribute__((always_inline))  set_axis( const vector& v, const float& f )
                    {
                        float  sin_theta , cos_theta ;
	                void vsincos( const float, float& , float& ) ;
                        vsincos( 0.5f*f , sin_theta, cos_theta ) ;
                        x = sin_theta * v.x;
                        y = sin_theta * v.y;
                        z = sin_theta * v.z;
                        w = cos_theta ;
                        return *this;
                    }

		

        private:
          inline void __attribute__((always_inline)) set( const matrix& m )
                    {
                      float tr, s;
                      size_t  i, j, k;

                      int nxt[3] = {1, 2, 0};

                      tr = m.trace();
                      if ( tr > 0.0)
                      {
                        s = vsqrt (tr + 1.0f);
                        w = s * 0.5f;
                        s = 0.5f / s;
                        x = (m(1,2) - m(2,1)) * s;
                        y = (m(2,0) - m(0,2)) * s;
                        z = (m(0,1) - m(1,0)) * s;
                      }
                      else
                      {
                        i = 0;
                        if (m.m11 > m.m00) i = 1;
                        if (m.m22 > m(i,i)) i = 2;
                        j = nxt[i];
                        k = nxt[j];

                        s = vsqrt ((m(i,i) - (m(j,j) + m(k,k))) + 1.0f);

                        q[i] = s * 0.5f;

                        if (s != 0.0f) s = 0.5 / s;

                        q[3] = (m(j,k) - m(k,j)) * s;
                        q[j] = (m(i,j) + m(j,i)) * s;
                        q[k] = (m(i,k) + m(k,i)) * s;

                        x = q[0];
                        y = q[1];
                        z = q[2];
                        w = q[3];
                      }
                    }

                inline matrix __attribute__((always_inline)) get() const
                    {
                      float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
                      x2 = x + x;
                      y2 = y + y;
                      z2 = z + z;
                      xx = x * x2;   xy = x * y2;   xz = x * z2;
                      yy = y * y2;   yz = y * z2;   zz = z * z2;
                      wx = w * x2;   wy = w * y2;   wz = w * z2;

                      return matrix(
                      1.0f-(yy+zz),         xy-wz,              xz+wy,
                      xy+wz,                1.0f-(xx+zz),       yz-wx,
                      xz-wy,                yz+wx,              1.0f-(xx+yy)
                                  );
                    }






                };
// conjucted quat
inline quat __attribute__((always_inline)) operator ~ (const quat& q)
		{
			quat tmp ( q ) ;
			return tmp.conj()  ;
		}

// inversed quat
inline quat __attribute__((always_inline)) operator ! (const quat& q)
		{
			quat tmp ( q ) ;
			return tmp.inv()  ;
		}

// vector product
inline quat __attribute__((always_inline)) operator * (const quat& q0 , const quat& q1)
		{
			quat tmp ( q0 ) ;
			return tmp*=q1  ;
		}

	// scalar product
	inline  float __attribute__((always_inline)) 
	operator ^ (quat& q0 , quat& q1 )
	{
  	return q0.x*q1.x + q0.y*q1.y + q0.z*q1.z + q0.w*q1.w ;
	}

	// get rotated vector qvq
	inline  vector __attribute__((always_inline)) 
	operator ^ (const quat& q , const vector& v )
	{
  		
		float v_coef = q.w * q.w - q.x * q.x - q.y * q.y - q.z * q.z;
        	float u_coef = 2.0f * (v.x * q.x + v.y * q.y + v.z * q.z );
        	float c_coef = 2.0f * q.w;

		return vector (
        	 v_coef * v.x + u_coef * q.x + c_coef * (q.y * v.z - q.z * v.y),
       		 v_coef * v.y + u_coef * q.y + c_coef * (q.z * v.x - q.x * v.z),
        	 v_coef * v.z + u_coef * q.z + c_coef * (q.x * v.y - q.y * v.x)
				);
	}

} 



#endif /* MATRIX3F_H_ */
