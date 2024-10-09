#ifndef __SUPC++_H__
#define __SUPC++_H__

#ifndef ERANGE
  //constexpr int ERANGE = 34;	/* Result too large */
  #define ERANGE  34
#endif
#include <stddef.h> // define size_t

extern "C" void *malloc( size_t WantedSize ) ;
extern "C" void  free( void* pv );

//------------------------------------------------------------------
// определение функции оператор new
inline void* operator new(size_t size) noexcept
  {
    return malloc( size );
  }
//------------------------------------------------------------------
// определение функции оператор delete
inline void operator delete(void* ptr) noexcept
  {
    free(ptr) ;
  }
inline void operator delete(void* ptr , size_t size) noexcept
  {
    free(ptr) ;
  }
//------------------------------------------------------------------
// определение функции оператор new[]
inline void* operator new[] (size_t size) noexcept
  {
    return malloc( size );
  }
//------------------------------------------------------------------
// определение функции оператор delete[]
inline void operator delete[] (void* ptr) noexcept
  {
    free(ptr) ;
  }
inline void operator delete[] (void* ptr, size_t size) noexcept
  {
    free(ptr) ;
  }
//------------------------------------------------------------------

__attribute__((__noreturn__)) inline void throw_exeption_catcher(const char* msg)
{
  volatile const char* tmp = msg ;
  (void)tmp ;
  while(1)
    {
       asm volatile("nop") ;
    }
}


// переопределение обработчиков исключений
#include <bits/c++config.h>
#include <bits/exception_defines.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  // Helper for exception objects in <except>
__attribute__((__noreturn__)) inline void
  __throw_bad_exception(void)
  {
    throw_exeption_catcher(0);
  }

  // Helper for exception objects in <new>
__attribute__((__noreturn__)) inline void
  __throw_bad_alloc(void)
  {
    throw_exeption_catcher(0);
  }

  // Helper for exception objects in <typeinfo>
__attribute__((__noreturn__)) inline void
  __throw_bad_cast(void)
  {
    throw_exeption_catcher(0);
  }

__attribute__((__noreturn__)) inline void
  __throw_bad_typeid(void)
  {
    throw_exeption_catcher(0);
  }

  // Helpers for exception objects in <stdexcept>
__attribute__((__noreturn__)) inline void
  __throw_logic_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_domain_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_invalid_argument(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_length_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_out_of_range(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__))
__attribute__((__format__(__printf__, 1, 2))) inline void
  __throw_out_of_range_fmt(const char* fmt, ...)
   {
      throw_exeption_catcher(fmt);
    }

__attribute__((__noreturn__)) inline void
  __throw_runtime_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_range_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_overflow_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_underflow_error(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

  // Helpers for exception objects in <ios>
__attribute__((__noreturn__)) inline void
  __throw_ios_failure(const char* msg)
  {
    throw_exeption_catcher(msg);
  }

__attribute__((__noreturn__)) inline void
  __throw_system_error(int)
  {
    throw_exeption_catcher(0);
  }

__attribute__((__noreturn__)) inline void
  __throw_future_error(int)
  {
    throw_exeption_catcher(0);
  }

  // Helpers for exception objects in <functional>
__attribute__((__noreturn__)) inline void
  __throw_bad_function_call()
  {
    throw_exeption_catcher(0);
  }

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace


namespace __gnu_cxx
{
  __attribute__((__noreturn__)) inline void
  __verbose_terminate_handler()
  {
    throw_exeption_catcher(0);
  }
};


#endif /* __SUPC++_H__ */
