#include <stddef.h>

//-----------------------------------------------------------
void* memcpy (void* dst0 , const void* src0 , size_t len0)
{
  char *dst = (char *) dst0;
  char *src = (char *) src0;

  char *save =  (char *)dst0;

  while (len0--)
    {
      *dst++ = *src++;
    }

  return save;
}
//-----------------------------------------------------------
void* memchr (const void* src_void, int c , size_t length)
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
void* memset (void* m , int c , size_t n)
{
  volatile char *s = (char *) m;
  while (n--)
     *s++ = (char) c;

   return m;
}
//-----------------------------------------------------------
int memcmp(const void *m1, const void *m2, size_t n)
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
void *memmove(void *dst_void, const void *src_void, size_t length)
{
  char *dst = dst_void;
  const char *src = src_void;

  if (src < dst && dst < src + length)
    {
      // Have to copy backwards
      src += length;
      dst += length;
      while (length--)
	{
	  *--dst = *--src;
	}
    }
  else
    {
      while (length--)
	{
	  *dst++ = *src++;
	}
    }

  return dst_void;
}
//-----------------------------------------------------------
