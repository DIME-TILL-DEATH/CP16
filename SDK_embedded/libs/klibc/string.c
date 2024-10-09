#include <stddef.h>

//-----------------------------------------------------------
__attribute__((used)) size_t strlen(const char* str)
{
  const char *start = str;

while (*str)
    str++;
  return str - start;

  }
//-----------------------------------------------------------
char* strncpy(char *__restrict dst0, const char *__restrict src0, size_t count)

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
__attribute__((used)) char* strcpy (char *dst0, const char *src0)
{
  char *s = dst0;

  while (*dst0++ = *src0++) ;

  return s;
  }
//-----------------------------------------------------------
__attribute__((used)) int strcmp (const char *s1 , const char *s2)
{
  while (*s1 != '\0' && *s1 == *s2)
    {
      s1++;
      s2++;
    }
  return (*(unsigned char *) s1) - (*(unsigned char *) s2);
  }

//-----------------------------------------------------------
__attribute__((used)) char * strchr(const char *s1 , int i)
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
size_t strspn(const char *s1, const char *s2)
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
char* strpbrk( const char *s1 , const char *s2)
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
    s1 = ((void *)0);

  return (char *) s1;
}
//----------------------------------------------------------
int strcoll(const char *a , const char *b)
{
  return strcmp (a, b);
}
//----------------------------------------------------------
char* strstr(const char *searchee, const char *lookfor)
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
