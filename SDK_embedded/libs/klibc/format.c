#include "int2str.h"
#include <math.h>

#include <stdarg.h>
#include <stdint.h>

typedef int (*send_char_fnc_t)(int c);

static void out_char(send_char_fnc_t send_char , char **str, int c)
{
	if (str)
	  if ( *str!=(char*)-1 )
	  {
		**str = c;
		++(*str);
		return ;
	  }

	(void)send_char(c);
}

#define PAD_RIGHT 1
#define PAD_ZERO 2

static int prints(send_char_fnc_t send_char , char **out, const char *string, int width, int pad)
{
	register int pc = 0, padchar = ' ';

	if (width > 0) {
		register int len = 0;
		register const char *ptr;
		for (ptr = string; *ptr; ++ptr) ++len;
		if (len >= width) width = 0;
		else width -= len;
		if (pad & PAD_ZERO) padchar = '0';
	}
	if (!(pad & PAD_RIGHT)) {
		for ( ; width > 0; --width) {
			out_char ( send_char , out, padchar);
			++pc;
		}
	}
	for ( ; *string ; ++string) {
	        out_char ( send_char , out, *string);
		++pc;
	}
	for ( ; width > 0; --width) {
	        out_char ( send_char , out, padchar);
		++pc;
	}

	return pc;
}



static int printi(send_char_fnc_t send_char , char **out, int i, int b, int sg, int width, int pad, int letbase)
{
        /* the following should be enough for 32 bit int */
        #define PRINT_BUF_LEN_INT32 32

        char print_buf[PRINT_BUF_LEN_INT32];
	register char *s;
	register int t, neg = 0, pc = 0;
	register unsigned int u = i;

	if (i == 0) {
		print_buf[0] = '0';
		print_buf[1] = '\0';
		return prints ( send_char , out, print_buf, width, pad);
	}

	if (sg && b == 10 && i < 0) {
		neg = 1;
		u = -i;
	}

	s = print_buf + PRINT_BUF_LEN_INT32-1;
	*s = '\0';

	while (u) {
		t = u % b;
		if( t >= 10 )
			t += letbase - '0' - 10;
		*--s = t + '0';
		u /= b;
	}

	if (neg) {
		if( width && (pad & PAD_ZERO) ) {
			out_char ( send_char, out, '-');
			++pc;
			--width;
		}
		else {
			*--s = '-';
		}
	}

	return pc + prints (send_char, out, s, width, pad);
}

/* the following should be enough for 32 bit int */


static int printd(send_char_fnc_t send_char , char **out, int64_t i, int b, int sg, int width, int pad, int letbase)
{
        /* the following should be enough for 64 bit int */
        #define PRINT_BUF_LEN_INT64 64

        char print_buf[PRINT_BUF_LEN_INT64];
        register char *s;
        register int t, neg = 0, pc = 0;
        register uint64_t u = i;

        if (i == 0) {
                print_buf[0] = '0';
                print_buf[1] = '\0';
                return prints ( send_char , out, print_buf, width, pad);
        }

        if (sg && b == 10 && i < 0) {
                neg = 1;
                u = -i;
        }

        s = print_buf + PRINT_BUF_LEN_INT64-1;
        *s = '\0';

        while (u) {
                t = u % b;
                if( t >= 10 )
                        t += letbase - '0' - 10;
                *--s = t + '0';
                u /= b;
        }

        if (neg) {
                if( width && (pad & PAD_ZERO) ) {
                        out_char ( send_char, out, '-');
                        ++pc;
                        --width;
                }
                else {
                        *--s = '-';
                }
        }

        return pc + prints (send_char, out, s, width, pad);
}

double modf   ( const double val , double* int_part) { double tmp = (int64_t)val ; *int_part = tmp ; return val - tmp ; }

int kprint( send_char_fnc_t send_char , char **out, const char *format, va_list args )
{
	int width, pad;
	int pc = 0;
	char scr[2];
	int float_fierst_arg = -1 ;

	for (; *format != 0; ++format) {
		if (*format == '%') {
			++format;
			width = pad = 0;
			if (*format == '\0') break;
			if (*format == '%') goto out;
			if (*format == '-') {
				++format;
				pad = PAD_RIGHT;
			}
			while (*format == '0') {
				++format;
				pad |= PAD_ZERO;
			}
			for ( ; *format >= '0' && *format <= '9'; ++format) {
				width *= 10;
				width += *format - '0';
			}

			if( *format == 'f' )
				{
				if ( float_fierst_arg==-1 )float_fierst_arg = 1 ;

					double intpart , fracpart ;
					if ( float_fierst_arg ==0 )
						args.__ap+=4 ;

					if ( *((double*)(args.__ap)) < 0.0)
						 pc+= prints (send_char, out, "-",0,0);

					fracpart = modf( fabs(*((double*)(args.__ap))) , &intpart ) ;
					args.__ap+= 8 ;

					pc+= printi (send_char, out, (int) intpart, 10, 0, 0, pad, 'a');
					pc+= prints (send_char, out, ".", 0, pad);
					for ( int frac_digit = 0 ; frac_digit < (width ? width : 16)  ; frac_digit++ )
						{
							fracpart *= 10.0 ;
							modf( fracpart , (double*)&intpart ) ;
							pc+= printi (send_char, out, (int)intpart , 10, 0, 0, pad, 'a');
							fracpart -= intpart ;
						}
				}

			if ( float_fierst_arg==-1 )float_fierst_arg = 0 ;

			if( *format == 's' ) {
				char *s = (char *)va_arg( args, int );
				pc += prints (send_char, out, s?s:"(null)", width, pad);
			}
			if( *format == 'd' ) {
				pc += printi (send_char, out, va_arg( args, int ), 10, 1, width, pad, 'a');
			}
			if( *format == 'l' ) {
			    pc += printi (send_char, out, va_arg( args, long int ), 10, 1, width, pad, 'a');
			}
			if( *format == 'x' ) {
				pc += printi (send_char, out, va_arg( args, int ), 16, 0, width, pad, 'a');
			}
			if( *format == 'X' ) {
				pc += printi (send_char, out, va_arg( args, int ), 16, 0, width, pad, 'A');
			}
			if( *format == 'p' ) {
			        pc += prints (send_char, out, "0x", width, pad);
			        pc += printi (send_char, out, va_arg( args, int ), 16, 0, width, pad, 'a');
			}
			if( *format == 'P' ) {
			        pc += prints (send_char, out, "0x", width, pad);
			        pc += printi (send_char, out, va_arg( args, int ), 16, 0, width, pad, 'A');
			                        }
			if( *format == 'u' ) {
				pc += printi (send_char, out, va_arg( args, int ), 10, 0, width, pad, 'a');
			}

			if( *format == 'U' )
			      pc += printd (send_char, out, va_arg( args, int64_t ), 10, 0, width, pad, 'a');

			if( *format == 'D' )
			      pc += printd (send_char, out, va_arg( args, int64_t ), 10, 1, width, pad, 'a');

                        if( *format == 'b' ) {
                                pc += printi (send_char, out, va_arg( args, int ), 2, 0, width, pad, 'a');
                        }

			if( *format == 'c' ) {
				/* char are converted to int then pushed on the stack */
				scr[0] = (char)va_arg( args, int );
				scr[1] = '\0';
				pc += prints (send_char, out, scr, width, pad);
			}
		}
		else {
		out:
			out_char (send_char, out, *format);
			++pc;
		}
	}
	if (out) **out = '\0';
	va_end( args );
	return pc;
}

int kprintf(send_char_fnc_t send_char , const char *format, ...)
{
   va_list args;
   va_start( args, format );
   int res = kprint( send_char, 0 , format, args );
   va_end(args) ;
   return res ;
}

int ksprintf(char *out, const char *format, ...)
{
  void __attribute__((weak,noreturn,used))  abort() ;

  if ( !out )
       abort();
   va_list args;
   va_start( args, format );
   int res = kprint( 0 , &out , format, args );
   va_end(args) ;
   return res ;
}

int kfprintf(send_char_fnc_t send_char , int *out, const char *format, ...)
{
  va_list args;
  va_start( args, format );
  // запись в консоль
  int res = kprint( send_char, 0 , format, args );
  va_end(args) ;
  return res ;
}

int kvsprintf (send_char_fnc_t send_char , char * s, const char * format, va_list arg )
{
	return  kprint( send_char, s ? &s : 0 , format, arg );
}

