#ifndef __EMB_PRINTF_H__
#define __EMB_PRINTF_H__

// KGP tools embedded SDK
// Chernov S.A. aka klen
// klen_s@mail.ru

#include <limits>
#include <string>

namespace kgp_sdk_libc
{
        inline std::string& trim_left(std::string& str)
           {
              str.erase(str.begin(), find_if(str.begin(), str.end(),
                [](char& ch)->bool { return !kgp_sdk_libc::isspace(ch); }));
              return str;
           }

        inline std::string& trim_right(std::string& str)
           {
              str.erase(find_if(str.rbegin(), str.rend(),
                [](char& ch)->bool { return !kgp_sdk_libc::isspace(ch); }).base(), str.end());
              return str;
           }

        inline std::string& trim(std::string& str)
           {
              str.erase(str.begin(), find_if(str.begin(), str.end(),
                [](char& ch)->bool { return !kgp_sdk_libc::isspace(ch); }));

              str.erase(find_if(str.rbegin(), str.rend(),
                [](char& ch)->bool { return !kgp_sdk_libc::isspace(ch); }).base(), str.end());
              return str;
           }

class emb_printf
{
   public:

      static inline void format(std::string& dest, const char *s)
        {
           while (*s)
             {
                if (*s == '%' && *(++s) != '%')
                   throw_exeption_catcher("invalid format std::string: missing arguments");
                dest += *s++ ;
             }
        }

      template<typename T, typename... Args>
      static inline void format(std::string& dest, const char *s, T value, Args... args)
        {
	   while (*s)
      	     {
                if (*s == '%' && *(++s) != '%')
                  {
        	    convert(dest, &s, value );
                    format(dest, s, args...); // продолжаем обработку аргументов, даже если *s == 0
                    return;
                  }
                dest += *s++ ;
             }
      	   throw_exeption_catcher("extra arguments provided to printf");
        }

      template<typename T, typename... Args>
      static void inline format(std::string& dest, const std::string& format, T value, Args... args)
        {
	   emb_printf::format(dest, format.c_str(), value, args...);
        }

      template<typename T, typename... Args>
      static inline void scatf(std::string& out , const char *format, T value, Args... args)
        {
           emb_printf::format(out, format, value, args... );
        }

      template<typename T, typename... Args>
      static inline void scatf(std::string& out , const std::string& format, T value, Args... args)
        {
           emb_printf::format(out, format.c_str(), value, args... );
        }

      template<typename T, typename... Args>
      static inline void sprintf(std::string& out , const char *format, T value, Args... args)
        {
           out.clear();
           emb_printf::format(out, format, value, args... );
        }

      template<typename T, typename... Args>
      static inline void sprintf(std::string& out , const std::string& format, T value, Args... args)
        {
           out.clear();
           emb_printf::format(out, format.c_str(), value, args... );
        }

   protected:

       //------------------------
       /* integer format:  %[-][*]n[X]
        *          * - space pad owerwrite '0'
        *          n - min digit pads, 0 - not out in zero val, [0...64]
        *          X - format
        *          	x or X - hexal
        *          	d - decimal
        *          	o - octal
        *          	b - binary
        *
        *
       */
       template <typename T>
       static inline void convert_integer(std::string& dest, const char** s, T val, bool float_neg = false)
       {
	    bool negative ;
	    // default arg 'float_neg' is used for resolve problem with -0.0f and +0.0f
	    // in convert_float->convert_integer  where float arg is value as abs(X) < 1.0f
	    if ( float_neg )
	      {
		// 'convert_float->convert_integer' path on negative float args in 'convert_float'
		negative = true ;
	      }
	    else
	      {
		// path for all integer and for positive float args
		negative = val < 0 ;
	      }

	    // for signum types
	    if (negative)   val=-val ;
	    uint8_t placeholder = 0 ;
	    uint8_t base = 0 ;
	    char pad_symbol = '0'   ;
	    bool pad_signum = false ;
	    bool hex_upper = false ;
	    const char val2hex[]="0123456789abcdef";


		     if ((**s)=='-')
			{
			  pad_signum = true;
			  (*s)++ ;
			}

		     if ((**s)=='*')
			{
			  pad_symbol = ' ';
			  (*s)++ ;
			}

		     if (!kgp_sdk_libc::isdigit(**s))  throw_exeption_catcher("invalid integer format for emb_printf::convert_unsigned");

	 	     placeholder = *((*s)++) -'0' ;

	             if (kgp_sdk_libc::isdigit(**s))   placeholder = placeholder*10 + *((*s)++) -'0' ;

	             // base system
	             switch( *((*s)++) )
	               {
	                 case 'X' :
	                   hex_upper = true ;
	                   base = 16 ;
	                   break ;
	                 case 'x' :
	                   base = 16 ;
	                   break ;
	                 case 'd' :
	                   base = 10 ;
	                   break ;
	                 case 'o' :
	                   base=8 ;
	                   break ;
	                 case 'b' :
	                   base = 2 ;
	                   break ;
	                 default:
	                   // default format base 10
	                   base = 10 ;  (*s)-- ;
	               }

	         size_t len_befor = dest.length();

	         if ( val == 0 )
	           dest += '0' ;
	         else
	           while (val)
	              {
	                T frac = val%base ;
	                dest += (char)( hex_upper ?  kgp_sdk_libc::toupper(val2hex[frac]) : val2hex[frac] ) ;
	                val /= base;
	              }


	         if ( negative )
	           {
	             if ( pad_symbol == ' ' )
	               {
	        	 dest+='-';
	        	 if ( dest.length() <  (len_befor + placeholder) )
	        	    dest.resize( len_befor + placeholder , pad_symbol ) ;
	        	 if (pad_signum)  dest+=' ';
	               }
	             else
	               {
	        	 if ( dest.length() <  (len_befor + placeholder) )
	        	    dest.resize( len_befor + placeholder , pad_symbol ) ;
	        	 dest+='-';
	               }
	           }
	         else // positive
	           {

	                if ( dest.length() <  (len_befor + placeholder) )
                          dest.resize( len_befor + placeholder , pad_symbol ) ;

	                if (pad_signum) dest+=' ';
	           }

	         std::reverse( dest.begin() + len_befor , dest.end());

       }




        //------------------------
       /* float format:
        *    %[-][*]n.m
        *             '-' - signum pad
        *             '*' - space pad owerwrite '0'
        *              n - min digit pads, 0 - not out in zero val, [0...64]
        *              m - min digit pads, 0 - not out in zero val, [0...64]
        *
        *
        *    %[-]'e'[m]
        *             '-' - signum pad
        *             'e' - is a flag as sceintific form [-][0..9]e[-][0..9]
        *              m - digits after decimal point, [1...7 ] float
        *                                              [1...16] double
        *
        *
        */
       //template <typename T>
       static inline void convert_float (std::string& dest, const char** s, float val)
          {
	     convert_float_signum(dest, s, val);

	     if ( **s == 'e' )
	       {
		 (*s)++ ; // skip 'e'

		 char fract_fmt[] = "7" ;

		 if (kgp_sdk_libc::isdigit(**s))
		   {
		     fract_fmt[0] = **s ;
		     if ( *((*s)++) -'0' > 7 )
		            throw_exeption_catcher("invalid float decimal exponent format for emb_printf::convert_float");
		   }

		 union float_cast_t
		 {
		   uint32_t i ;
		   float f;
		   struct
		     {
		       unsigned int mantisa : 23;
		       unsigned int exponent : 8;
		       unsigned int sign : 1;
		     };

		 } float_cast = { .f= val };

		 if ( convert_float_is_nan_inf(dest, val ) ) return ;

		 val = __builtin_fabsf(val) ;



		 int32_t exp10 = kgp_sdk_libc::floor( (float_cast.exponent - 127) / 3.32192809489f );

		 if ( (exp10 == 0) || (exp10==-39))
		   {

		   }
		 else
		   {
		      float d = 10.0 ;
		      for( int32_t i = __builtin_fabsf(exp10) - 1 ; i > 0 ; i-- )
		   	 d *= 10.0f ;

		      exp10 > 0 ? val /= d : val *= d ;
	           }

		 if ( val >= 10.0f )
                	 {
                	   exp10++ ;
                	   val /= 10.0f ;
                	 }


		 uint32_t p1 = kgp_sdk_libc::floor(val) ;

		 float mul = 10.0f ;
		 for ( size_t i = fract_fmt[0] - '0' - 1; i > 0 ; i-- )
		      mul *= 10.0f ;

		 uint32_t p2 = kgp_sdk_libc::fract(val) * mul ;


            	 const char int_exp_fmt[] = "1" ;
		 const char* int_exp_fmt_ptr = int_exp_fmt ;

		 convert_integer( dest, &int_exp_fmt_ptr, (uint32_t)p1) ;
		 dest+='.' ;


		 const char* fract_ptr = fract_fmt;
		 convert_integer( dest, &fract_ptr, p2) ;

		 dest+='e' ;
		 int_exp_fmt_ptr = int_exp_fmt ;
		 convert_integer( dest, &int_exp_fmt_ptr, exp10) ;

	       }
	     else
		 convert_float_n_dot_m (dest, s, val);
            }


             //template <typename T>
        static inline void convert_float (std::string& dest, const char** s, double val)
            {

             convert_float_signum(dest, s, val);

	     if ( **s == 'e' )
	       {
		 (*s)++ ; // skip 'e'
       		 char fract_fmt[] = "16" ;

       		 if (kgp_sdk_libc::isdigit(**s))
       		   {
       		     fract_fmt[0] = *((*s)++)  ;

       		     if ( !kgp_sdk_libc::isdigit(**s))
       		       	{
       			   throw_exeption_catcher("invalid float decimal exponent format for emb_printf::convert_double");
       		       	}

       		     fract_fmt[1] = *((*s)++) ;

       		     if ( (10*(fract_fmt[0]-'0') + (fract_fmt[1] -'0')) > 16 )
       		            throw_exeption_catcher("invalid float decimal exponent format for emb_printf::convert_double");
       		   }

       		 union double_cast_t
       		   {
       		     uint64_t i ;
       		     double f;
       		     struct
       		       {
       		         uint64_t mantisa  : 52;
       		         uint64_t exponent : 11;
       		         uint64_t sign : 1;
       		     };

       		 } double_cast = { .f= val };

         	if ( convert_float_is_nan_inf(dest, val) ) return ;

       		 val = __builtin_fabs(val) ;

       		 int32_t exp10 = kgp_sdk_libc::floor( (double_cast.exponent - 1023) / 3.32192809489 );

       		 if ( exp10 == 0 )
       		   {

       		   }
       		 else
       		   {
       		      double d = 10.0 ;
       		      for( int32_t i = __builtin_fabs(exp10) - 1 ; i > 0 ; i-- )
       		   	 d *= 10.0f ;

       		      exp10 > 0 ? val /= d : val *= d ;
       	           }

       		 if ( val >= 10.0 )
                       	 {
                       	   exp10++ ;
                       	   val /= 10.0 ;
                       	 }


       		 uint32_t p1 = kgp_sdk_libc::floor(val) ;

       		 double mul = 10.0 ;
       		 for ( size_t i = 10*(fract_fmt[0] - '0') + (fract_fmt[1] - '0') - 1; i > 0 ; i-- )
       		      mul *= 10.0 ;

       		 uint64_t p2 = kgp_sdk_libc::fract(val) * mul ;

       		 const char int_exp_fmt[] = "1" ;
       		 const char* int_exp_fmt_ptr = int_exp_fmt ;

       		 convert_integer( dest, &int_exp_fmt_ptr, p1) ;
       		 dest+='.' ;


       		 const char* fract_ptr = fract_fmt;
       		 convert_integer( dest, &fract_ptr, p2) ;

       		 dest+='e' ;
       		 int_exp_fmt_ptr = int_exp_fmt ;
       		 convert_integer( dest, &int_exp_fmt_ptr, exp10) ;

       	       }
       	     else
       	       convert_float_n_dot_m (dest, s, val);
 }

       //------------------------
       static void inline convert ( std::string& dest, const char** s, const uint8_t       val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const uint16_t      val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const uint32_t      val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const uint64_t      val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const unsigned int  val )  { convert_integer( dest, s, val ) ; }

       //------------------------
       static void inline convert ( std::string& dest, const char** s, const int8_t  val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const int16_t val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const int32_t val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const int64_t val )  { convert_integer( dest, s, val ) ; }
       static void inline convert ( std::string& dest, const char** s, const int     val )  { convert_integer( dest, s, val ) ; }

       //------------------------
       static void inline convert ( std::string& dest, const char** s, const float val )    { convert_float (dest, s, val); }
       static void inline convert ( std::string& dest, const char** s, const double val )   { convert_float (dest, s, val); }

       //------------------------
       static void inline convert ( std::string& dest, const char** s, const void* val )    { convert_integer( dest, s, (unsigned int)val ); }

       static void inline convert ( std::string& dest, const char** s, const std::string& val )  { (*s)++ ; dest += val ; }
       static void inline convert ( std::string& dest, const char** s, const char* val)         { (*s)++ ; dest += std::string(val) ; }
       static void inline convert ( std::string& dest, const char** s, const char  val)         { (*s)++ ; dest.append(1,val) ;      }

       emb_printf() {} ;

   private:

       template <typename T>
       static inline bool convert_float_is_nan_inf(std::string& dest,T & val)
              {
                ;
	         if ( is_nan(val))  { dest+="nan" ; return true; }
                 if ( is_inf(val))  { dest+="inf" ; return true; }
	         return false ;
              }

       template <typename T>
       static inline void convert_float_signum(std::string& dest, const char** s, T& val)
       {
	  // вывод знака для всех форматов
	  bool sign_placeholder = false ;
	  if (**s == '-')
	     {
		sign_placeholder = true ;
	 	(*s)++;
	     }

	  if ( val < 0.0f )
	    {
	      dest+='-' ;
	      val = -val ;
	    }
          else
    	      if ( sign_placeholder )
    	          dest+=' ' ;
       }

       //------------------------
       /* float format:
        *    %[-][*]n.m
        *             '-' - signum pad
        *             '*' - space pad owerwrite '0'
        *              n - min digit pads, 0 - not out in zero val, [0...64]
        *              m - min digit pads, 0 - not out in zero val, [0...64]
        *              */
        template <typename T>
        static inline void convert_float_n_dot_m (std::string& dest, const char** s, T val)
        {
           size_t tmp = dest.size() ;

           // Extract integer part
           int32_t ipart = val;

           // convert integer part to std::string
           convert_integer( dest, s, ipart ) ;

           // skip "." symbol
           dest+='.';
           (*s)++   ;

           // read frac pads
           size_t frac_pads = 0 ;
           if (!kgp_sdk_libc::isdigit(**s))  throw_exeption_catcher("invalid integer format for emb_printf::convert_unsigned");
           frac_pads = *((*s)++) -'0' ;
           if (kgp_sdk_libc::isdigit(**s))   frac_pads = frac_pads*10 + *((*s)++) -'0' ;

           float frac = (val-ipart) < 0 ? -(val-ipart) : val-ipart ;

           while ( frac_pads-- )
            {
              frac = frac*((float)10) ;
              uint32_t digit = (uint32_t)frac ;
              frac = frac - (float)digit ;
              dest+= char('0' + digit) ;
            }

           if ( is_nan(val))
             {
               dest.resize(tmp) ;
               convert_float_is_nan_inf(dest, val);
             }

         }
};


//------------------------
#ifndef EMB_STREAM_STRING_INTEGER_FORMAT
#define EMB_STREAM_STRING_INTEGER_FORMAT "%1"
#endif

inline std::string& operator << ( std::string& dest, const uint8_t       val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const uint16_t      val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const uint32_t      val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const uint64_t      val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const unsigned int  val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }

inline std::string& operator << ( std::string& dest, const int8_t  val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const int16_t val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const int32_t val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const int64_t val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const int     val )  { emb_printf::scatf(dest, EMB_STREAM_STRING_INTEGER_FORMAT , val); return dest; }



//------------------------
#ifndef EMB_STREAM_STRING_FLOAT_FORMAT
#define EMB_STREAM_STRING_FLOAT_FORMAT "%-e2"
#endif

#ifndef EMB_STREAM_STRING_DOUBLE_FORMAT
#define EMB_STREAM_STRING_DOUBLE_FORMAT "%-e02"
#endif

inline std::string& operator << ( std::string& dest, const float val  )   { emb_printf::scatf(dest, EMB_STREAM_STRING_FLOAT_FORMAT , val); return dest; }
inline std::string& operator << ( std::string& dest, const double val )   { emb_printf::scatf(dest, EMB_STREAM_STRING_DOUBLE_FORMAT , val); return dest; }

//------------------------
#ifndef EMB_STREAM_STRING_POINTER_FORMAT
#define EMB_STREAM_STRING_POINTER_FORMAT "%1x"
#endif
inline std::string& operator << ( std::string& dest, const void* val )    { emb_printf::scatf(dest, EMB_STREAM_STRING_POINTER_FORMAT , val); return dest; }

inline std::string& operator << ( std::string& dest, const std::string& val )  { dest+= val ;return dest; }
inline std::string& operator << ( std::string& dest, const char* val)     { dest.append(val) ; return dest; }
inline std::string& operator << ( std::string& dest, const char  val)     { dest.append(1,val) ; return dest; }


} // kgp_sdk_libc




#endif
