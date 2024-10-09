#ifndef __INT2STR__
#define __INT2STR__

#ifdef __cplusplus
extern "C" {
#endif

//char* int2str ( char* buff , unsigned int val , int radix , int fixed , int sym ) ;



const char* i2str (  const int val , const char* buff , const unsigned int buf_len , const unsigned char radix  , const unsigned char fixed );
const char* ui2str ( const int val , const char* buff , const unsigned int buf_len , const unsigned char radix , const unsigned char fixed );
const char* float2str(  double val , const char* buff , const unsigned int buff_len, const unsigned char int_fixed  ,const unsigned char fract_fixed );


#ifdef __cplusplus
			}
#endif

#endif __INT2STR__
