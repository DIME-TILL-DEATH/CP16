#include "int2str.h"
#include "string.h"
#include <math.h> 

char* int2str ( char* buff , unsigned int val , int radix , int fixed , int syms )
{
  static const char table[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'} ;
  unsigned int tmp = val ;
  if (fixed)
	  memset (buff , '0' , syms ) ;
  buff[syms] = 0 ;

  int pos = syms ;
  do
   {
    pos--;
    buff[pos] = table[ tmp % radix ] ;
    tmp /= radix ;
   } while (tmp) ;

  if (fixed)
	  return buff + syms - fixed ;
  else
	  return buff + pos ;
}

//---------------------------------------------------------------------------------------
static char* convstr (  const char* buff_end , const unsigned int val , const unsigned int radix  )
{
	char* sym = (char*)buff_end;
	unsigned int tmp = val ;
	static const char table[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'} ;
	*sym--=0;
	while ( 1 )
	 {
		 unsigned int quot  = tmp / radix ;
		 unsigned int remin = tmp % radix ;
		 *sym =  table[remin] ;
		 if ( !quot ) break ;
		 tmp = quot ;
		 sym-- ;
	 }
	return sym ;
}
//---------------------------------------------------------------------------------------
const char* i2str ( const int val , const char* buff , const unsigned int buff_len, const unsigned char radix , const unsigned char fixed )
{
	char* conv_result = convstr ( buff + buff_len , val > 0 ? val : -val , radix ) ;
	unsigned int len = strlen(conv_result) ;
	if ( len < fixed )
		{
			memset ( conv_result - fixed + len , '0' , fixed - len ) ;
			conv_result = conv_result - fixed + len ;
		}

	 if ( val < 0)
		 *--conv_result='-' ;
	 else if (fixed) *--conv_result=' ' ;

	 return conv_result ;
}
//---------------------------------------------------------------------------------------
const char* ui2str ( const int val , const char* buff , const unsigned int buff_len, const unsigned char radix , const unsigned char fixed  )
{
	char* conv_result = convstr ( buff + buff_len , val , radix ) ;
	unsigned int len = strlen(conv_result) ;
	if ( len < fixed )
	{
		memset ( conv_result - fixed + len , '0' , fixed - len ) ;
		conv_result = conv_result - fixed + len ;
	}
	return conv_result ;
}
//---------------------------------------------------------------------------------------
const char* float2str(  double val , const char* buff , const unsigned int buff_len, const unsigned char int_fixed , const unsigned char fract_fixed )
{
 	const double scaler[] = { 1e0 , 1e1 , 1e2 , 1e3 , 1e4 , 1e5 , 1e6 , 1e7 , 1e8 , 1e9,  1e10}  ;

	double intpart  ;
    double fracpart ;
	fracpart = fabs(modf( val , (double*)&intpart )) ;

	const char* conv_result = ui2str ( (unsigned int)(fracpart * scaler[fract_fixed] ) , buff , buff_len, 10  , fract_fixed  ) ;
	char* dot_ptr = (char*)conv_result - 1 ;

	conv_result = i2str ( (int)  intpart , buff , dot_ptr - buff , 10  , int_fixed  ) ;

	*dot_ptr='.' ;

	return conv_result ;
}

