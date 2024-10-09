#ifndef __KLIBC++_H__
#define __KLIBC++_H__

// KGP tools embedded SDK
// Chernov S.A. aka klen
// klen_s@mail.ru

#include <cstdint>
#include <cstddef>
#include <climits>
#include <limits>

namespace kgp_sdk_libc
{
  static inline bool is_nan(float val)
     {
        union ifu_t  { uint32_t ui ; float f ; } v,i ;
      	v.f = val ;
      	v.ui &= 0x7fffffff  ; // сброс знакового бита
  	i.f = std::numeric_limits<decltype(ifu_t::f)>::quiet_NaN() ;
        return v.ui == i.ui ;
     }

  static inline bool is_nan(double val)
     {
        union ifu_t  { uint64_t ui ; double f ; } v,i ;
      	v.f = val ;
      	v.ui &= 0x7fffffffffffffff  ; // сброс знакового бита
  	i.f = std::numeric_limits<decltype(ifu_t::f)>::quiet_NaN() ;
        return v.ui == i.ui ;
     }

  static inline bool is_inf(float val)
     {
        union ifu_t  { uint32_t ui ; float f ; } v,i ;
      	v.f = val ;
      	v.ui &= 0x7fffffff  ; // сброс знакового бита
  	i.f = std::numeric_limits<decltype(ifu_t::f)>::infinity() ;
        return v.ui == i.ui ;
     }



  static inline bool is_inf(double val)
     {
        union ifu_t  { uint64_t ui ; double f ; } v,i ;
      	v.f = val ;
      	v.ui &= 0x7fffffffffffffff  ; // сброс знакового бита
  	i.f = std::numeric_limits<decltype(ifu_t::f)>::infinity() ;
        return v.ui == i.ui ;
     }

#if 0  // TODO конфликутет c climits->includ-fixed/limits.h
  // ------------ limits -----------------------------
  /* Number of bits in a `char'.  */
  constexpr int8_t CHAR_BIT = 8 ;
  constexpr int8_t SCHAR_MIN = -128;
  constexpr int8_t SCHAR_MAX = 127;

  /* Maximum value an `unsigned char' can hold.  (Minimum is 0).  */
  constexpr uint8_t UCHAR_MAX = 255 ;

  /* Minimum and maximum values a `signed short int' can hold.  */
  constexpr int16_t SHRT_MIN =(-32767-1);
  constexpr int16_t SHRT_MAX = 32767;

  /* Maximum value an `unsigned short int' can hold.  (Minimum is 0).  */
  constexpr uint16_t USHRT_MAX = 65535;

  /* Minimum and maximum values a `signed int' can hold.  */
  constexpr int32_t __INT_MAX__ = 2147483647;
  constexpr int32_t INT_MAX  = __INT_MAX__;
  constexpr int32_t INT_MIN  = (-INT_MAX-1);


  /* Maximum value an `unsigned int' can hold.  (Minimum is 0).  */
  constexpr uint32_t UINT_MAX  = (INT_MAX * 2U + 1);

  /* Minimum and maximum values a `signed long int' can hold.
     (Same as `int').  */
  constexpr int32_t __LONG_MAX__  = 2147483647L;

  constexpr int32_t LONG_MAX  = __LONG_MAX__;
  constexpr int32_t LONG_MIN  = (-LONG_MAX-1);


  /* Maximum value an `unsigned long int' can hold.  (Minimum is 0).  */
  constexpr uint32_t ULONG_MAX  = (LONG_MAX * 2UL + 1);

  constexpr int64_t __LONG_LONG_MAX__ = 9223372036854775807LL;

  /* Minimum and maximum values a `signed long long int' can hold.  */
  constexpr int64_t LLONG_MAX  = __LONG_LONG_MAX__;
  constexpr int64_t LLONG_MIN  = (-LLONG_MAX-1);


  /* Maximum value an `unsigned long long int' can hold.  (Minimum is 0).  */
  constexpr uint64_t ULLONG_MAX  = (LLONG_MAX * 2ULL + 1);


  /* Minimum and maximum values a `signed long long int' can hold.  */
  constexpr int64_t LONG_LONG_MAX  = __LONG_LONG_MAX__;
  constexpr int64_t LONG_LONG_MIN  = (-LONG_LONG_MAX-1);


  /* Maximum value an `unsigned long long int' can hold.  (Minimum is 0).  */
  constexpr uint64_t ULONG_LONG_MAX  = (LONG_LONG_MAX * 2ULL + 1);
#endif

  //----------- reentrant ----------------------------
  #ifdef __USE_REENTRANT__
     struct reent_t
       {
         inline reent_t() : err(0), rand(0), strtok(0) {}
         int          err   ;
         unsigned int rand  ;
         char *       strtok ;
       } ;


      // имплементация структуры и функций доступа
      #ifdef __KLIBC_WRAPS_IMPL__

         reent_t* klibc_reent ;

         // функции предназначены исключительно для вызова планировщиком FreeRTOS, поэтому имеют интерфейс "C"
         extern "C" void  reent_set(void* val)        { klibc_reent  = (reent_t*)val ; }
         extern "C" void* reent_get()                 { return (void*)klibc_reent ; }
         extern "C" void  reent_init (void** val)     { *((reent_t**) val) = new reent_t ; }
         extern "C" void  reent_deinit   (void** val) { delete *((reent_t**) val) ; }
      #else
         // объявление
         extern reent_t* klibc_reent ;
      #endif /*__KLIBC_WRAPS_IMPL__*/
   #endif __USE_REENTRANT__
  //--------------------------------------------------




  //----------- errno ----------------------------
   constexpr int EPERM = 1;		/* Not owner */
   constexpr int ENOENT = 2;	/* No such file or directory */
   constexpr int ESRCH = 3;		/* No such process */
   constexpr int EINTR = 4;		/* Interrupted system call */
   constexpr int EIO = 5;		/* I/O error */
   constexpr int ENXIO = 6;		/* No such device or address */
   constexpr int E2BIG = 7;		/* Arg list too long */
   constexpr int ENOEXEC = 8;	/* Exec format error */
   constexpr int EBADF = 9;		/* Bad file number */
   constexpr int ECHILD = 10;	/* No children */
   constexpr int EAGAIN = 11;	/* No more processes */
   constexpr int ENOMEM = 12;	/* Not enough space */
   constexpr int EACCES = 13;	/* Permission denied */
   constexpr int EFAULT = 14;	/* Bad address */

   constexpr int EBUSY = 16;	/* Device or resource busy */
   constexpr int EEXIST = 17;	/* File exists */
   constexpr int EXDEV = 18;	/* Cross-device link */
   constexpr int ENODEV = 19;	/* No such device */
   constexpr int ENOTDIR = 20;	/* Not a directory */
   constexpr int EISDIR = 21;	/* Is a directory */
   constexpr int EINVAL = 22;	/* Invalid argument */
   constexpr int ENFILE = 23;	/* Too many open files in system */
   constexpr int EMFILE = 24;	/* File descriptor value too large */
   constexpr int ENOTTY = 25;	/* Not a character device */
   constexpr int ETXTBSY = 26;	/* Text file busy */
   constexpr int EFBIG = 27;	/* File too large */
   constexpr int ENOSPC = 28;	/* No space left on device */
   constexpr int ESPIPE = 29;	/* Illegal seek */
   constexpr int EROFS = 30;	/* Read-only file system */
   constexpr int EMLINK = 31;	/* Too many links */
   constexpr int EPIPE = 32;	/* Broken pipe */
   constexpr int EDOM = 33;		/* Mathematics argument out of domain of function */
#ifndef ERANGE
   constexpr int ERANGE = 34;	/* Result too large */
#endif
   constexpr int ENOMSG = 35;	/* No message of desired type */
   constexpr int EIDRM = 36;	/* Identifier removed */



   constexpr int EDEADLK = 45;	/* Deadlock */
   constexpr int ENOLCK = 46;	/* No lock */



   constexpr int  ENOSTR = 60;	/* Not a stream */
   constexpr int  ENODATA = 61;	/* No data (for no delay io) */
   constexpr int  ETIME = 62;	/* Stream ioctl timeout */
   constexpr int  ENOSR = 63;	/* No stream resources */



   constexpr int  ENOLINK = 67;	/* Virtual circuit is gone */

   constexpr int  EPROTO = 71;	/* Protocol error */
   constexpr int  EMULTIHOP = 74;	/* Multihop attempted */

   constexpr int  EBADMSG = 77;	/* Bad message */
   constexpr int  EFTYPE = 79;	/* Inappropriate file type or format */


   constexpr int  ENOSYS = 88;	/* Function not implemented */



   constexpr int  ENOTEMPTY = 90;	/* Directory not empty */
   constexpr int  ENAMETOOLONG = 91;	/* File or path name too long */
   constexpr int  ELOOP = 92;	/* Too many symbolic links */
   constexpr int  EOPNOTSUPP = 95;	/* Operation not supported on socket */
   constexpr int  EPFNOSUPPORT = 96; /* Protocol family not supported */
   constexpr int  ECONNRESET = 104;  /* Connection reset by peer */
   constexpr int  ENOBUFS = 105;	/* No buffer space available */
   constexpr int  EAFNOSUPPORT = 106; /* Address family not supported by protocol family */
   constexpr int  EPROTOTYPE = 107;	/* Protocol wrong type for socket */
   constexpr int  ENOTSOCK = 108;	/* Socket operation on non-socket */
   constexpr int  ENOPROTOOPT = 109;	/* Protocol not available */



   constexpr int  ECONNREFUSED = 111;	/* Connection refused */
   constexpr int  EADDRINUSE = 112;		/* Address already in use */
   constexpr int  ECONNABORTED = 113;	/* Software caused connection abort */
   constexpr int  ENETUNREACH = 114;		/* Network is unreachable */
   constexpr int  ENETDOWN = 115;		/* Network interface is not configured */
   constexpr int  ETIMEDOUT = 116;		/* Connection timed out */
   constexpr int  EHOSTDOWN = 117;		/* Host is down */
   constexpr int  EHOSTUNREACH = 118;	/* Host is unreachable */
   constexpr int  EINPROGRESS = 119;		/* Connection already in progress */
   constexpr int  EALREADY = 120;		/* Socket already connected */
   constexpr int  EDESTADDRREQ = 121;	/* Destination address required */
   constexpr int  EMSGSIZE = 122;		/* Message too long */
   constexpr int  EPROTONOSUPPORT = 123;	/* Unknown protocol */



   constexpr int  EADDRNOTAVAIL = 125;	/* Address not available */
   constexpr int  ENETRESET = 126;		/* Connection aborted by network */
   constexpr int  EISCONN = 127	;	/* Socket is already connected */
   constexpr int  ENOTCONN = 128;		/* Socket is not connected */
   constexpr int  ETOOMANYREFS = 129;



   constexpr int  EDQUOT = 132;
   constexpr int  ESTALE = 133;
   constexpr int  ENOTSUP = 134	;	/* Not supported */



   constexpr int  EILSEQ = 138;		/* Illegal byte sequence */
   constexpr int  EOVERFLOW = 139;	/* Value too large for defined data type */
   constexpr int  ECANCELED = 140;	/* Operation canceled */
   constexpr int  ENOTRECOVERABLE = 141;	/* State not recoverable */
   constexpr int  EOWNERDEAD = 142;	/* Previous owner died */


   constexpr int  EWOULDBLOCK = EAGAIN;	/* Operation would block */

   constexpr int  __ELASTERROR = 2000;	/* Users can add values starting here */

   #ifdef __USE_REENTRANT__


         inline void errno_set(const int val) { klibc_reent->err = val ; }
         inline int  errno_get() { return klibc_reent->err ; }

         // выплясывание перед C++::cerrno на предмет того что он тянет С-like дефайн 'errno *(__errno())'
         #define errno *(  &(klibc_reent->err)  )

   #else
         #ifdef __KLIBC_WRAPS_IMPL__
              int klibc_errno ;
         #else
              extern int klibc_errno ;
         #endif

         inline void errno_set(const int val) { klibc_errno = val ; }
         inline int  errno_get() { return klibc_errno ; }

         // выплясывание перед C++::cerrno на предмет того что он тянет С-like дефайн 'errno *(__errno())'
         #define errno klibc_errno

   #endif


   //-----------------------------------------------




  template <typename T>
  inline T abs (T val) { return val < (T)0 ? -val : val ; }

  //--------------- ctype ----------------------------------------------
  inline int isascii(const int c) { return (c & 0x80) == 0; }
  inline int isblank(const int c) { return c == ' ' || c == '\t'; }
  inline int isdigit(const int c) { return c >= '0' && c <= '9'; }
  inline int iscntrl(const int c) { return c < 32; }
  inline int islower(const int c) { return c >= 'a' && c <= 'z'; }
  inline int isspace(const int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
  inline int isupper(const int c) { return c >= 'A' && c <= 'Z'; }
  inline int isxdigit(const int c) { return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

  inline int isalpha(const int c) { return isupper(c) || islower(c); }
  inline int isalnum(const int c) { return isalpha(c) || isdigit(c); }
  inline int isgraph(const int c) { return !iscntrl(c) && !isspace(c); }
  inline int isprint(const int c) { return !iscntrl(c); }
  inline int ispunct(const int c) { return !iscntrl(c) && !isspace(c) && !isalnum(c); }

  inline int toupper(const int c) { return islower(c) ? c & ~0x20 : c; }
  inline int tolower(const int c) { return isupper(c) ? c | 0x20 : c; }
  //-------------------------------------------------------------


#if 1  // все ХОРОШО но для флота достаточно приведения  (int)val  то для дабла (int64)val  :(  шаблон пока не катит
/*
1. round (х) не реализован как floor (х + 0,5), так как это дает косяг при х = 0,5-2^-54
2. предполагается, что логические операции преобразуются в целочисленные значения 1 для true и 0 для false
3. нет гаранти что это всегда правильно работает... за то быстро
*/
template <typename T>
inline T modf  ( const T val , T& int_part) { T tmp = (int64_t)val ; int_part = tmp ; return val - tmp ; }
template <typename T>
inline T fract  ( const T val) { return val  - (int64_t)val ; }
template <typename T>
inline T floor(T x) {  return int64_t(x) - (  x<(T)0   && (fract(x) != (T)0) ) ; }
template <typename T>
inline T ceil(T x)  {  return int64_t(x) + (  x>(T)0 && (fract(x) != (T)0)  ); }
template <typename T>
inline T round(T x) { return floor(x)+( x>(T)0 && fract(x)>=((T)0.5)) + fract(( x<(T)0 && ((T)1+ fract(x) )) >= ((T)0.5)) ; }

#else

inline auto modf   ( const float val , float& int_part) { float tmp = (int32_t)val ; int_part = tmp ; return val - tmp ; }
inline auto fract  ( const float val)                   { return val  - (int32_t)val ; }
inline auto floor  ( float x)                           { return int32_t(x) - (  x<(int32_t)0 && (fract(x) != (int32_t)0) ); }
inline auto ceil   ( float x)                           { return int32_t(x) + (  x>(int32_t)0 && (fract(x) != (int32_t)0) ); }
inline auto round  ( float x)                           { return floor(x)+( x>0 && fract(x)>=((float)0.5)) + fract(float(( x<0 && (1+ fract(x) )) >= ((float)0.5))) ; }

inline auto modf   ( const double val , double& int_part) { double tmp = (int64_t)val ; int_part = tmp ; return val - tmp ; }
inline auto fract  ( const double val)                  { return val  - (int64_t)val ; }
inline auto floor  ( double x)                          { return int64_t(x) - (  x<(int64_t)0 && (fract(x) != (int64_t)0) ); }
inline auto ceil   ( double x)                          { return int64_t(x) + (  x>(int64_t)0 && (fract(x) != (int64_t)0) ); }
inline auto round  ( double x)                          { return floor(x)+( x>0 && fract(x)>=((double)0.5)) + fract(double(( x<0 && (1+ fract(x) )) >= ((double)0.5))) ; }


#endif

//----------------------------------------------------------------------------
//                        STRING FUNCTION
//----------------------------------------------------------------------------

inline void* memcpy (void* dst0 , const void* src0 , size_t len0)
 {
   volatile char  *dst = (char *) dst0;
   char  *src = (char *) src0;
   char  *save =  (char *)dst0;
   while (len0--)
      *dst++ = *src++;
   return save ;
  }


//inline void memcpy32 (void* dst0 , const void* src0 , size_t len0)
// {
//   len0 >>= 2  ;
//   volatile unsigned int* dst = (volatile unsigned int*)dst0 ;
//   const unsigned int* src = (const unsigned int*)src0 ;
//   while (len0--)
//         *dst++ = *src++;
// }

//-----------------------------------------------------------
inline void* memccpy(void* dst, const void* src, int c, size_t count)
{
  char *a = (char*)dst;
  const char *b = (const char*)src;
  while (count--)
  {
    *a++ = *b;
    if (*b==c)
    {
      return (void *)a;
    }
    b++;
  }
  return 0;
}

//-----------------------------------------------------------
inline void* memchr (const void* src_void, int c , size_t length)
{
  const unsigned char *src = (const unsigned char *) src_void;
  unsigned char d = c;
  while (length--)
    {
      if (*src == d)
        return (void *) src;
      src++;
    }

  return NULL;
}
//-----------------------------------------------------------
      inline void* memset (void* m , int c , size_t n)
      {
        volatile char *s = (char *) m;
        while (n--)
           *s++ = (char) c;

        return m;
      }

      inline void wmemset (void* dst0 , const int c , size_t len0)
      {
	   len0 >>= 2  ;
	   volatile unsigned int* dst = (volatile unsigned int*)dst0 ;
	   while (len0--)
	         *dst++ = c;
      }

//-----------------------------------------------------------
inline int memcmp(const void* m1, const void* m2, size_t n)
{
  unsigned char *s1 = (unsigned char *) m1;
  unsigned char *s2 = (unsigned char *) m2;

  while (n--)
    {
      if (*s1 != *s2)
	{
	  return *s1 - *s2;
	}
      s1++;
      s2++;
    }
  return 0;
}
//-----------------------------------------------------------
inline void *memmove(void* dst_void, const void* src_void, size_t length)
      {
         char* dst = (char*)dst_void;
         const char* src = (const char*)src_void;

         if (src < dst && dst < src + length)
           {
             // Have to copy backwards
             src += length;
             dst += length;
             while (length--)
      	        *--dst = *--src;
           }
         else
           {
             while (length--)
      	        *dst++ = *src++;

          }

        return dst_void;
      }

//-----------------------------------------------------------
inline size_t strlen(const char* str)
{
   const char *start = str;
   while (*str)
          str++;
   return str - start;
}
//-----------------------------------------------------------
inline char* stpcpy (char *dst, const char *src)
{
    while ((*dst++ = *src++));
    return (dst-1);
}
//-----------------------------------------------------------
inline char* stpncpy(char* dest, const char* src, size_t n)
{
  char* x=(char*)memccpy(dest,src,0,n);
  if (x)
    {
       memset(x,0,n-(x-dest));
       return x-1;
    }
  else
    return dest+n;
}
//-----------------------------------------------------------
inline char* strncpy(char* __restrict dst0, const char* __restrict src0, size_t count)

{
  char *dscan;
  const char *sscan;

  dscan = dst0;
  sscan = src0;
  while (count > 0)
    {
      --count;
      if ((*dscan++ = *sscan++) == '\0')
	break;
    }
  while (count-- > 0)
    *dscan++ = '\0';

  return dst0;
}
//-----------------------------------------------------------
inline char* strcpy (char* __restrict dst0, const  char* __restrict src0)
{
  char *s = dst0;

  while ( (*dst0++ = *src0++) ) ;

  return s;
  }
//-----------------------------------------------------------
inline size_t strlcpy(char* __restrict dst, const char* __restrict src, size_t count)
{
	const char *osrc = src;
	size_t nleft = count;

	/* Copy as many bytes as will fit. */
	if (nleft != 0) {
		while (--nleft != 0) {
			if ((*dst++ = *src++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src. */
	if (nleft == 0) {
		if (count != 0)
			*dst = '\0';		/* NUL-terminate dst */
		while (*src++)
			;
	}

	return(src - osrc - 1);	/* count does not include NUL */
}
//-----------------------------------------------------------
inline int strcmp (const char* s1 , const char* s2)
{
  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
  }
//-----------------------------------------------------------
inline int strncmp(const char* s1, const char* s2, size_t n)
{
  if (n == 0)
    return 0;

  while (n-- != 0 && *s1 == *s2)
    {
      if (n == 0 || *s1 == '\0')
        break;
      s1++;
      s2++;
    }

  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
}
//-----------------------------------------------------------
inline int  strcasecmp ( const char* s1, const char* s2 )
{
    unsigned int  x2;
    unsigned int  x1;

    while (1) {
        x2 = *s2 - 'A'; if (__builtin_expect(x2 < 26u, 0)) x2 += 32;
        x1 = *s1 - 'A'; if (__builtin_expect(x1 < 26u ,0)) x1 += 32;
	s1++; s2++;
        if (__builtin_expect(x2 != x1, 0) )
            break;
        if (__builtin_expect(x1 == (unsigned int)-'A',0) )
            break;
    }

    return x1 - x2;
}
//---------------------------------------------------------
inline int strncasecmp(const char *s1, const char *s2, size_t n)
{
	unsigned char c = 1U;

	for (; c && n != 0; n--) {
		unsigned char lower1, lower2;

		c = *s1++;
		lower1 = tolower(c);
		lower2 = tolower(*s2++);

		if (lower1 != lower2) {
			return (lower1 > lower2) - (lower1 < lower2);
		}
	}

	return 0;
}
//-----------------------------------------------------------
inline char* strchr(const char* s1 , const int i)
{
  const unsigned char *s = (const unsigned char *)s1;
  unsigned char c = i;

  while (*s && *s != c)
      s++;
    if (*s == c)
      return (char *)s;
    return NULL;
}
//-----------------------------------------------------------
inline size_t strspn(const char* s1, const char* s2)
{
    const char *s = s1;
    const char *c;

    while (*s1)
      {
        for (c = s2; *c; c++)
  	{
  	  if (*s1 == *c)
  	    break;
  	}
        if (*c == '\0')
  	break;
        s1++;
      }

    return s1 - s;
  }
//-----------------------------------------------------------
inline char* strpbrk( const char* s1 , const char* s2)
{
  const char *c = s2;
  if (!*s1)
    return (char *) NULL;

  while (*s1)
    {
      for (c = s2; *c; c++)
	{
	  if (*s1 == *c)
	    break;
	}
      if (*c)
	break;
      s1++;
    }

  if (*c == '\0')
    s1 = 0;

  return (char *) s1;
}
//----------------------------------------------------------
inline int strcoll(const char* a , const char* b)
{
  return strcmp (a, b);
}
//----------------------------------------------------------
inline char* strstr(const char* searchee, const char* lookfor)
{
  /* Less code size, but quadratic performance in the worst case.  */
  if (*searchee == 0)
    {
      if (*lookfor)
	return (char *) 0;
      return (char *) searchee;
    }

  while (*searchee)
    {
      size_t i;
      i = 0;

      while (1)
	{
	  if (lookfor[i] == 0)
	    {
	      return (char *) searchee;
	    }

	  if (lookfor[i] != searchee[i])
	    {
	      break;
	    }
	  i++;
	}
      searchee++;
    }

  return (char *) 0;
}
//-------------------------------------------------------------------
inline char* strcat(char* s, const char* t)
{
  char *dest=s;
  s+=strlen(s);
  for (;;)
    {
     if (!(*s = *t))
       break;
     ++s;
     ++t;
    }
  return dest;
}
//-------------------------------------------------------------------
inline size_t strlcat(char* dst, const char* src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (*d != '\0' && n-- != 0)
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}
//-------------------------------------------------------------------
inline size_t strcspn(const char* s, const char* reject)
{
  size_t l=0;
  int i;

  for (; *s; ++s) {
    for (i=0; reject[i]; ++i)
      if (*s==reject[i]) return l;
    ++l;
  }
  return l;
}
//-------------------------------------------------------------------
inline char* strerror(int errnum)
{
  while (1)
    {
      __asm__ volatile ("nop");
    }
  return (char*)"Not implement :( ";
}
//-------------------------------------------------------------------
inline char *strncat(char* s, const char* t, size_t n)
{
  char *dest=s;
  char *max;
  s+=strlen(s);
  if (__builtin_expect((max=s+n)==s,0)) goto fini;
  for (;;)
    {
      if (__builtin_expect(!(*s = *t),0))
        break;
      if (__builtin_expect(++s==max,0))
        break;
      ++t;
    }
  *s=0;
 fini:
  return dest;
}
//-------------------------------------------------------------------



inline char *strtok(char* s,const char* delim)
{
  char*tmp=0;

  #ifdef __USE_REENTRANT__
     char* pos  = klibc_reent->strtok;
  #else
     static char* pos  = 0;
  #endif

  if (s==0)
    s=pos;
  s+=strspn(s,delim);		/* overread leading delimiter */
  if (*s) [[likely]]
    {
      tmp=s;
      s+=strcspn(s,delim);
      if (*s) [[likely]] *s++=0;	/* not the end ? => terminate it */
    }
  pos=s;
  return tmp;
}

//-------------------------------------------------------------------
inline size_t strxfrm(char* dest, const char* src, size_t n)
{
    memset(dest,0,n);
    memccpy(dest,src,0,n);
    return strlen(dest);
}
//-------------------------------------------------------------------
inline char *strrchr(const char* t, int c) {
  char ch;
  const char *l=0;

  ch = c;
  for (;;) {
    if (__builtin_expect(*t == ch,0))
      l=t;
    if (__builtin_expect(!*t,0))
      return (char*)l;
    ++t;
  }
  return (char*)l;
}
//-------------------------------------------------------------------
inline double strtod(const char* s, char** endptr) {
    const char* p = s;
    long double value = 0.L;
    int sign = +1;
    long double factor;
    unsigned int expo;

    while ( isspace(*p) )
        p++;

    switch (*p) {
    case '-': sign = -1; p++ ; break ;
    case '+': p++; break ;
    default : break;
    }

    while ( (unsigned int)(*p - '0') < 10u )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            factor *= 0.1;
            value += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo = 0;
        factor = 10.L;

        switch (*++p) { // ja hier weiß ich nicht, was mindestens nach einem 'E' folgenden MUSS.
        case '-': factor = 0.1; p++; break ;
        case '+': p++; break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p = s;
                  goto done;
        }

        while ( (unsigned int)(*p - '0') < 10u )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != ((void *)0) )
        *endptr = (char*)p;

    return value * sign;
}
//----------------------------------------------------------------------------
inline float strtof(const char* s, char** endptr) {
    const char*  p     = s;
    float        value = 0.;
    int                   sign  = +1;
    float                 factor;
    unsigned int          expo;

    while ( isspace( (int)*p ) )
        p++;

    switch (*p) {
    case '-': sign = -1; p++; break ;
    case '+': p++;
    default : break;
    }

    while ( (unsigned int)(*p - '0') < 10u )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.L;

        switch (*++p) {                 // ja hier weiß ich nicht, was mindestens nach einem 'E' folgenden MUSS.
        case '-': factor = 0.1; p++; break;
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p     = s;
                  goto done;
        }

        while ( (unsigned int)(*p - '0') < 10u )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != (void*) 0 )
        *endptr = (char*)p;

    return value * sign;
}
//----------------------------------------------------------------------------
template <typename T>
inline T strto(const char* s, char** endptr) {
    const char*  p     = s;
    T   value = 0.;
    int sign  = +1;
    T   factor;
    unsigned int  expo;

    while ( isspace( (int)*p ) )
        p++;

    switch (*p) {
    case '-': sign = -1; p++; break ;
    case '+': p++;
    default : break;
    }

    while ( (unsigned int)(*p - '0') < 10u )
        value = value*10 + (*p++ - '0');

    if ( *p == '.' ) {
        factor = 1.;

        p++;
        while ( (unsigned int)(*p - '0') < 10u ) {
            factor *= 0.1;
            value  += (*p++ - '0') * factor;
        }
    }

    if ( (*p | 32) == 'e' ) {
        expo   = 0;
        factor = 10.L;

        switch (*++p) {                 // ja hier weiß ich nicht, was mindestens nach einem 'E' folgenden MUSS.
        case '-': factor = 0.1; p++; break;
        case '+': p++;
                  break;
        case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                  break;
        default : value = 0.L;
                  p     = s;
                  goto done;
        }

        while ( (unsigned int)(*p - '0') < 10u )
            expo = 10 * expo + (*p++ - '0');

        while ( 1 ) {
            if ( expo & 1 )
                value *= factor;
            if ( (expo >>= 1) == 0 )
                break;
            factor *= factor;
        }
    }

done:
    if ( endptr != (void*) 0 )
        *endptr = (char*)p;

    return value * sign;
}
// ---------------------------------------------------------------------------
inline long strtol(const char *__restrict nptr, char **__restrict endptr, int base=10)
{
	unsigned long acc;
	unsigned char c;
	unsigned long cutoff;
	signed char any;
	unsigned char flag = 0;
        constexpr int  FL_NEG = 0x01	;	/* number is negative */
        constexpr int  FL_0X = 0x02;	/* number has a 0x prefix */

	if (endptr)
		*endptr = (char *)nptr;
	if (base != 0 && (base < 2 || base > 36))
		return 0;

	/*
	 * Skip white space and pick up leading +/- sign if any.
	 * If base is 0, allow 0x for hex and 0 for octal, else
	 * assume decimal; if base is already 16, allow 0x.
	 */
	do {
		c = *nptr++;
	} while (isspace(c));
	if (c == '-') {
		flag = FL_NEG;
		c = *nptr++;
	} else if (c == '+')
		c = *nptr++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*nptr == 'x' || *nptr == 'X')) {
		c = nptr[1];
		nptr += 2;
		base = 16;
		flag |= FL_0X;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;

	/*
	 * Compute the cutoff value between legal numbers and illegal
	 * numbers.  That is the largest legal value, divided by the
	 * base.  An input number that is greater than this value, if
	 * followed by a legal input character, is too big.  One that
	 * is equal to this value may be valid or not; the decision
	 * about this is done as outlined below.
	 *
	 * Overflow detections works as follows:
	 *
	 * As:
	 *    acc_old <= cutoff
	 * then:
	 *    acc_old * base <= 0x80000000        (unsigned)
	 * then:
	 *    acc_old * base + c <= 0x80000000 + c
	 * or:
	 *    acc_new <= 0x80000000 + 35
	 *
	 * i.e. carry from MSB (by calculating acc_new) is impossible
	 * and we can check result directly:
	 *
	 *    if (acc_new > 0x80000000) then overflow
	 *
	 * Set any if any `digits' consumed; make it negative to indicate
	 * overflow.
	 */
#if  LONG_MIN != -LONG_MAX - 1
#  error "This implementation of strtol() does not work on this platform."
#endif
	switch (base) {
	    case 10:
		cutoff = ((unsigned long)LONG_MAX + 1) / 10;
		break;
	    case 16:
		cutoff = ((unsigned long)LONG_MAX + 1) / 16;
		break;
	    case 8:
		cutoff = ((unsigned long)LONG_MAX + 1) / 8;
		break;
	    case 2:
		cutoff = ((unsigned long)LONG_MAX + 1) / 2;
		break;
	    default:
		cutoff = ((unsigned long)LONG_MAX + 1) / base;
	}

	for (acc = 0, any = 0;; c = *nptr++) {
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff) {
			any = -1;
			continue;
		}
		acc = acc * base + c;
		if (acc > (unsigned long)LONG_MAX + 1)
			any = -1;
		else
			any = 1;
	}
	if (endptr) {
		if (any)
		    *endptr = (char *)nptr - 1;
		else if (flag & FL_0X)
		    *endptr = (char *)nptr - 2;
	}
	if (any < 0) {
		acc = (flag & FL_NEG) ? LONG_MIN : LONG_MAX;
		errno_set(ERANGE);
	} else if (flag & FL_NEG) {
		acc = -acc;
	} else if ((signed long)acc < 0) {
		acc = LONG_MAX;
		errno_set(ERANGE);
	}
	return (acc);
}
//----------------------------------------------------------------------------
inline unsigned long strtoul(const char *__restrict nptr, char **__restrict endptr, int base=10)
{
	unsigned long acc;
	unsigned char c;
	unsigned long cutoff;
	signed char any;
	char flag = 0;
        constexpr int  FL_NEG =	0x01 ;		/* number is negative */
        constexpr int  FL_0X =	0x02 ;		/* number has a 0x prefix */
	if (endptr)
		*endptr = (char *)nptr;
	if (base != 0 && (base < 2 || base > 36))
		return 0;
	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *nptr++;
	} while (isspace(c));
	if (c == '-') {
		flag = FL_NEG;
		c = *nptr++;
	} else if (c == '+')
		c = *nptr++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*nptr == 'x' || *nptr == 'X')) {
		c = nptr[1];
		nptr += 2;
		base = 16;
		flag |= FL_0X;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	/*
	 * cutoff computation is similar to strtol().
	 *
	 * Description of the overflow detection logic used.
	 *
	 * First, let us assume an overflow.
	 *
	 * Result of `acc_old * base + c' is cut to 32 bits:
	 *  acc_new <-- acc_old * base + c - 0x100000000
	 *
	 *  `acc_old * base' is <= 0xffffffff   (cutoff control)
	 *
	 * then:   acc_new <= 0xffffffff + c - 0x100000000
	 *
	 * or:     acc_new <= c - 1
	 *
	 * or:     acc_new < c
	 *
	 * Second:
	 * if (no overflow) then acc * base + c >= c
	 *                        (or: acc_new >= c)
	 * is clear (alls are unsigned).
	 *
	 */
	switch (base) {
		case 16:    cutoff = ULONG_MAX / 16;  break;
		case 10:    cutoff = ULONG_MAX / 10;  break;
		case 8:     cutoff = ULONG_MAX / 8;   break;
		default:    cutoff = ULONG_MAX / base;
	}
	for (acc = 0, any = 0;; c = *nptr++) {
		if (c >= '0' && c <= '9')
			c -= '0';
		else if (c >= 'A' && c <= 'Z')
			c -= 'A' - 10;
		else if (c >= 'a' && c <= 'z')
			c -= 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0)
			continue;
		if (acc > cutoff) {
			any = -1;
			continue;
		}
		acc = acc * base + c;
		any = (c > acc) ? -1 : 1;
	}
	if (endptr) {
		if (any)
			*endptr = (char *)nptr - 1;
		else if (flag & FL_0X)
			*endptr = (char *)nptr - 2;
	}
	if (flag & FL_NEG)
		acc = -acc;
	if (any < 0) {
		acc = ULONG_MAX;
		errno_set(ERANGE);
	}
	return (acc);
}
//-------------------------------------------------------------------------
inline unsigned long long int strtoull(const char *__restrict ptr, char **__restrict endptr, int base=10)
{
  int neg = 0, overflow = 0;
  long long int v=0;
  const char* orig;
  const char* nptr=ptr;

  while(isspace(*nptr)) ++nptr;

  if (*nptr == '-') { neg=1; nptr++; }
  else if (*nptr == '+') ++nptr;
  orig=nptr;
  if (base==16 && nptr[0]=='0') goto skip0x;
  if (base) {
    unsigned int b=base-2;
    if (b>34) { errno_set(EINVAL); return 0; }
  } else {
    if (*nptr=='0') {
      base=8;
skip0x:
      if (((*(nptr+1)=='x')||(*(nptr+1)=='X')) && isxdigit(nptr[2])) {
	nptr+=2;
	base=16;
      }
    } else
      base=10;
  }
  while(*nptr) {
    unsigned char c=*nptr;
    c=(c>='a'?c-'a'+10:c>='A'?c-'A'+10:c<='9'?c-'0':0xff);
    if (c>=base) break;	/* out of base */
    {
      unsigned long x=(v&0xff)*base+c;
      unsigned long long w=(v>>8)*base+(x>>8);
      if (w>(ULLONG_MAX>>8)) overflow=1;
      v=(w<<8)+(x&0xff);
    }
    ++nptr;
  }
  if (nptr==orig) {		/* no conversion done */
    nptr=ptr;
    errno_set(EINVAL);
    v=0;
  }
  if (endptr) *endptr=(char *)nptr;
  if (overflow) {
      errno_set(ERANGE);
    return ULLONG_MAX;
  }
  return (neg?-v:v);
}
//-------------------------------------------------------------------
inline long long int strtoll(const char *__restrict nptr, char **__restrict endptr, int base=10)
{
  int neg=0;
  unsigned long long int v;
  const char*orig=nptr;

  while(isspace(*nptr)) nptr++;

  if (*nptr == '-' && isalnum(nptr[1])) { neg=-1; nptr++; }
  v=strtoull(nptr,endptr,base);
  if (endptr && *endptr==nptr) *endptr=(char *)orig;
  if (v>LLONG_MAX) {
    if (v==0x8000000000000000ull && neg) {
	errno_set(0);
      return v;
    }
    errno_set(ERANGE);
    return (neg?LLONG_MIN:LLONG_MAX);
  }
  return (neg?-v:v);
}
//----------------------------------------------------------------------------------
// URL: https://web.archive.org/web/20100312021317/http://www.jb.man.ac.uk/~slowe/cpp/itoa.html

inline char* itoa(int value, char* result, const int base=10)
   {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = '\0';
	while(ptr1 < ptr)
	  {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	  }
	return result;
   }

inline char* uitoa(unsigned int value, char* result, const unsigned int base=10)
   {
	// check that the base if valid
	if (base < 2 || base > 36) { *result = '\0'; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	unsigned int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
	} while ( value );

	*ptr-- = '\0';
	while(ptr1 < ptr)
	  {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	  }
	return result;
   }

//----------------------------------------------------------------------------------
// URL: https://github.com/antongus/stm32tpl/blob/master/ftoa.c
/*  stm32tpl --  STM32 C++ Template Peripheral Library
*  Visit https://github.com/antongus/stm32tpl for new versions
*
*  Copyright (c) 2011-2020 Anton B. Gusev aka AHTOXA
*/

inline char * ftoa(double f, char * buf, int precision)
{
  constexpr size_t max_precision = 10 ;

  static constexpr double rounders[max_precision + 1] =
  {
  	0.5,				// 0
  	0.05,				// 1
  	0.005,				// 2
  	0.0005,				// 3
  	0.00005,			// 4
  	0.000005,			// 5
  	0.0000005,			// 6
  	0.00000005,			// 7
  	0.000000005,		// 8
  	0.0000000005,		// 9
  	0.00000000005		// 10
  };

        char * ptr = buf;
	char * p = ptr;
	char * p1;
	char c;
	long intPart;

	// check precision bounds
	if ( precision > (int)max_precision)
		precision = max_precision;

	// sign stuff
	if (f < 0)
	{
		f = -f;
		*ptr++ = '-';
	}

	if (precision < 0)  // negative precision == automatic precision guess
	{
		if (f < 1.0) precision = 6;
		else if (f < 10.0) precision = 5;
		else if (f < 100.0) precision = 4;
		else if (f < 1000.0) precision = 3;
		else if (f < 10000.0) precision = 2;
		else if (f < 100000.0) precision = 1;
		else precision = 0;
	}

	// round value according the precision
	if (precision)
		f += rounders[precision];

	// integer part...
	intPart = f;
	f -= intPart;

	if (!intPart)
		*ptr++ = '0';
	else
	{
		// save start pointer
		p = ptr;

		// convert (reverse order)
		while (intPart)
		{
			*p++ = '0' + intPart % 10;
			intPart /= 10;
		}

		// save end pos
		p1 = p;

		// reverse result
		while (p > ptr)
		{
			c = *--p;
			*p = *ptr;
			*ptr++ = c;
		}

		// restore end pos
		ptr = p1;
	}

	// decimal part
	if (precision)
	{
		// place decimal point
		*ptr++ = '.';

		// convert
		while (precision--)
		{
			f *= 10.0;
			c = f;
			*ptr++ = '0' + c;
			f -= c;
		}
	}

	// terminating zero
	*ptr = 0;

	return buf;
}





//----------------------- rand.c ---------------------------------------
#ifdef __USE_REENTRANT__
   inline void  srand(unsigned seed) { klibc_reent->rand = seed % (1<<31) ; }
#else
   static unsigned int klibc_rand ;
   inline void  srand(unsigned seed) { klibc_rand = seed % (1<<31) ; }
#endif
//-----------------------------------------------------------------------
#ifdef __USE_REENTRANT__
   inline int  rand() {  klibc_reent->rand = ( 1103515245 * klibc_reent->rand + 12345 ) & 0x7fffffff; return klibc_reent->rand ; }
#else
   inline int  rand() { klibc_rand = ( 1103515245 * klibc_rand + 12345 ) & 0x7fffffff; return klibc_rand ; }
#endif


inline float  randf() { return rand () / ((float)0x7fffffff) ; }
inline double randd() { return rand () / ((double)0x7fffffffffffffff) ; }
//-----------------------------------------------------------------------

// c++ error handler , that is invoked when in function calls exceptions caused
inline __attribute__((noreturn,used))  void abort()
{
	 while(1) {}
}
//-----------------------------------------------------------------------
inline  __attribute__ ((noreturn,used)) void __assert_func (const char* file, int line, const char* func, const char* error)
{
  (void)file ;
  (void)line ;
  (void)func ;
  (void)error ;
  while(1) {}
}

//----------------------------- KGP extensions --------------------------
template <typename T>
inline T middle_of_three(  T a,  T b,  T c )
{
if (a > b)
    {
        if (b > c) return b;
        else return a > c ?  c : a ;
    }
    else
    {
        if (a > c) return a;
        else return b > c ?  c : b ;
    }
}

template <typename T, typename P>
inline void increment_wrap ( const T wraper, P & counter  )
   {
       counter = counter + 1 ;
       if ( counter >= wraper)  counter = 0 ;
   }

template <typename T, typename P>
inline void decrement_wrap ( const T wraper, P & counter  )
   {
       counter = counter + 1 ;
       if ( counter == static_cast<P>(-1))  counter = wraper - 1 ;
   }

} ;


using namespace kgp_sdk_libc ;

// C-like support with wrapers
#ifdef __KLIBC_WRAPS_IMPL__
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) int     memcmp(const void* m1, const void* m2, size_t n)             { return kgp_sdk_libc::memcmp (m1, m2, n); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) void*   memcpy (void *__restrict dst0 , const void *__restrict src0 , size_t len0)
                                                                                                                                             { return kgp_sdk_libc::memcpy (dst0, src0 , len0); }

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char*   strncpy(char* __restrict dst0, const char* __restrict src0, size_t count)
                                                                                                                                             { return kgp_sdk_libc::strncpy(dst0, src0, count); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char *  stpcpy (char* __restrict dst, const char   *src)   { return kgp_sdk_libc::stpcpy(dst, src); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char*   stpncpy(char* __restrict dst, const char* __restrict src, size_t n)
                                                                                                                                             { return kgp_sdk_libc::stpncpy(dst, src, n); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char*   strcpy (char* __restrict dst, const char* __restrict src)
                                                                                                                                             { return kgp_sdk_libc::strcpy(dst, src); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) size_t  strlcpy(char* __restrict dst, const char* __restrict src, size_t n)
                                                                                                                                             { return kgp_sdk_libc::strlcpy(dst, src, n); }

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) size_t  strlen (const char* str)                                     { return kgp_sdk_libc::strlen(str); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) int     strcmp (const char* s1 , const char* s2)                     { return kgp_sdk_libc::strcmp (s1, s2); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) int     strncmp(const char* s1, const char* s2, size_t n)            { return kgp_sdk_libc::strncmp(s1, s2, n); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char*   strcat (char* s, const char* t)                              { return kgp_sdk_libc::strcat(s, t); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) void*   memset (void* m, int c, size_t n)                            { return kgp_sdk_libc::memset(m, c, n); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) void*   memmove(void* dst_void, const void* src_void, size_t length) { return kgp_sdk_libc::memmove(dst_void, src_void, length); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) void*   memchr (const void* src_void, int c , size_t length)         { return kgp_sdk_libc::memchr (src_void, c , length); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) double  strtod (const char* s, char** endptr)                        { return kgp_sdk_libc::strtod(s, endptr);}
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) float   strtof (const char* s, char** endptr)                        { return kgp_sdk_libc::strtof(s, endptr);}
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) long    strtol (const char *__restrict ptr, char **__restrict endptr, int base)
                                                                                                                                             { return kgp_sdk_libc::strtol(ptr, endptr,base);}
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) size_t  strspn (const char* s1, const char* s2)                      { return kgp_sdk_libc::strspn(s1, s2);}
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) size_t  strcspn(const char* s, const char* reject)                   { return kgp_sdk_libc::strcspn(s, reject);}
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char*   strchr(const char* s , const int i)                          { return kgp_sdk_libc::strchr(s, i);}



      extern "C" __attribute__ (( externally_visible, weak/*, used */)) unsigned long strtoul(const char *__restrict ptr, char **__restrict endptr, int base)
                                                                                                                                             { return kgp_sdk_libc::strtoul(ptr, endptr,base);}

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) unsigned long long int strtoull(const char *__restrict ptr, char **__restrict endptr, int base)
                                                                                                                                             { return kgp_sdk_libc::strtoull(ptr, endptr,base);}

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) long long int strtoll (const char *__restrict ptr, char **__restrict endptr, int base)
                                                                                                                                             { return kgp_sdk_libc::strtoll(ptr, endptr,base);}

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char* itoa(int val, char* buf, int base)                             { return kgp_sdk_libc::itoa(val, buf, base); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char* uitoa(int val, char* buf, int base)                            { return kgp_sdk_libc::uitoa(val, buf, base); }
      extern "C" __attribute__ (( externally_visible, weak/*, used */)) char* ftoa(double val, char * buf, int precision)                    { return kgp_sdk_libc::ftoa(val, buf, precision);}

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) void abort ()                                                        { kgp_sdk_libc::abort (); }

      extern "C" __attribute__ (( externally_visible, weak/*, used */)) int rand ()                                                          { return kgp_sdk_libc::rand (); }

      // catcher's

      extern "C"  __attribute__(( externally_visible, weak/*, used */, noreturn)) int atexit( void (*func)() )                                         { std::__throw_bad_function_call();  }
      //extern "C"  void _Unwind_Resume (void *)                                                                                             { std::__throw_bad_function_call(); }

      extern "C" __attribute__ (( externally_visible, weak/*, used */, noreturn)) void __assert_func (const char* file, int line, const char* func, const char* error)
                                                                                                                                             { kgp_sdk_libc::__assert_func (file, line, func, error); }


#endif /*__KLIBC_WRAPS_IMPL__*/

#endif /* __KLIBC++_H__ */
