#ifndef __SUPSTL_H__
#define __SUPSTL_H__

#include <limits>
#include <algorithm>
#include <cstring>
#include <string>


extern "C" void* malloc( size_t ) ;
extern "C" void  free( void* ) ;
extern "C" int ksprintf(char *out, const char *format, ...);

namespace std
{


using std::size_t;
using std::ptrdiff_t;

/**
 *  @brief  An allocator that uses malloc.
 *  @ingroup allocators
 *
 *  This is precisely the allocator defined in the C++ Standard.
 *    - all allocation calls malloc
 *    - all deallocation calls free
 */
template<typename _Tp>
  class KgpAllocator
  {
  public:
    typedef size_t     size_type;
    typedef ptrdiff_t  difference_type;
    typedef _Tp*       pointer;
    typedef const _Tp* const_pointer;
    typedef _Tp&       reference;
    typedef const _Tp& const_reference;
    typedef _Tp        value_type;

    template<typename _Tp1>
      struct rebind
      { typedef KgpAllocator<_Tp1> other; };

    KgpAllocator() _GLIBCXX_USE_NOEXCEPT { }

    KgpAllocator(const KgpAllocator&) _GLIBCXX_USE_NOEXCEPT { }

    template<typename _Tp1>
    KgpAllocator(const KgpAllocator<_Tp1>&)
      _GLIBCXX_USE_NOEXCEPT { }

    ~KgpAllocator() _GLIBCXX_USE_NOEXCEPT { }

    pointer
    address(reference __x) const _GLIBCXX_NOEXCEPT
    { return std::__addressof(__x); }

    const_pointer
    address(const_reference __x) const _GLIBCXX_NOEXCEPT
    { return std::__addressof(__x); }

    // NB: __n is permitted to be 0.  The C++ standard says nothing
    // about what the return value is when __n == 0.
    pointer
    allocate(size_type __n, const void* = 0)
    {
      if (__n > this->max_size())
        std::__throw_bad_alloc();

      pointer __ret = static_cast<_Tp*>(malloc(__n * sizeof(_Tp)));
      if (!__ret)
        std::__throw_bad_alloc();
      return __ret;
    }

    // __p is not permitted to be a null pointer.
    void
    deallocate(pointer __p, size_type)
    { free(static_cast<void*>(__p)); }

    size_type
    max_size() const _GLIBCXX_USE_NOEXCEPT
    { return size_t(-1) / sizeof(_Tp); }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    template<typename _Up, typename... _Args>
      void
      construct(_Up* __p, _Args&&... __args)
      { ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

    template<typename _Up>
      void
      destroy(_Up* __p) { __p->~_Up(); }
#else
    // _GLIBCXX_RESOLVE_LIB_DEFECTS
    // 402. wrong new expression in [some_] allocator::construct
    void
    construct(pointer __p, const _Tp& __val)
    { ::new((void *)__p) value_type(__val); }

    void
    destroy(pointer __p) { __p->~_Tp(); }
#endif
  };

template<typename _Tp>
  inline bool
  operator==(const KgpAllocator<_Tp>&, const KgpAllocator<_Tp>&)
  { return true; }

template<typename _Tp>
  inline bool
  operator!=(const KgpAllocator<_Tp>&, const KgpAllocator<_Tp>&)
  { return false; }


//-------------------------------------------------------------------------------------


        typedef basic_string<char, char_traits<char>, KgpAllocator<char> > emb_string;   /// A string of @c char

        inline emb_string operator+(const emb_string& s, /*const T rv*/const size_t rv )
                {
                  emb_string str;
                  char buff[16] ;
                  ksprintf( buff , "%d" , rv ) ;
                  str = s + emb_string (buff) ;
                  return str ;
                }

        inline emb_string operator+(const size_t rv , const emb_string& s )
                {
                  emb_string str;
                  char buff[16] ;
                  ksprintf( buff , "%d" , rv ) ;
                  str = emb_string (buff) + s ;
                  return str ;
                }

        inline emb_string operator+(const emb_string& s, /*const T rv*/const float rv )
                        {
                          emb_string str;
                          char buff[32] ;
                          ksprintf( buff , "%7f" , rv ) ;
                          str = s + emb_string (buff) ;
                          return str ;
                        }

        inline emb_string operator+(const float rv , const emb_string& s )
                        {
                          emb_string str;
                          char buff[32] ;
                          ksprintf( buff , "%7f" , rv ) ;
                          str = emb_string (buff) + s ;
                          return str ;
                        }

        inline emb_string&  operator+= (emb_string& lv, const size_t rv)
                {
                  char buff[16] ;
                  ksprintf( buff , "%d" , rv ) ;
                  lv += emb_string (buff) ;
                  return lv ;
                }

        inline emb_string&  operator+= (emb_string& lv, const float rv)
                {
                  char buff[32] ;
                  ksprintf( buff , "%7f" , rv ) ;
                  lv += emb_string (buff) ;
                  return lv ;
                }
        inline emb_string&  operator+= (emb_string& lv, const double rv)
                {
                  return lv += (float)rv ;
                }
/*
        class KgpStringCompare
        {
                public:
                        inline bool operator() (const char* lhs, const char* rhs) const {return strcmp(lhs,rhs) < 0  ;}
                        inline bool operator() (const emb_string& lhs, const emb_string& rhs) const {return lhs==rhs ;}
        };
*/

        inline emb_string& trim_left(emb_string& str)
                {
                  str.erase(str.begin(), find_if(str.begin(), str.end(),
                    [](char& ch)->bool { return !isspace(ch); }));
                  return str;
                }

        inline emb_string& trim_right(emb_string& str)
                {
                  str.erase(find_if(str.rbegin(), str.rend(),
                    [](char& ch)->bool { return !isspace(ch); }).base(), str.end());
                  return str;
                }

        inline emb_string& trim(emb_string& str)
        {
          str.erase(str.begin(), find_if(str.begin(), str.end(),
            [](char& ch)->bool { return !isspace(ch); }));

          str.erase(find_if(str.rbegin(), str.rend(),
            [](char& ch)->bool { return !isspace(ch); }).base(), str.end());
          return str;
        }



        class KgpCompare
        {
                public:
                        inline bool operator() (const char lhs, const char rhs) const {return lhs < rhs ;}
                        inline bool operator() (const int& lhs, const int& rhs) const {return lhs < rhs ;}
                        inline bool operator() (const float& lhs, const float& rhs) const {return lhs < rhs ;}
                        inline bool operator() (const double& lhs, const double& rhs) const {return lhs < rhs ;}
                        inline bool operator() (const char* lhs, const char* rhs) const {return strcmp(lhs,rhs) < 0  ;}
                        inline bool operator() (const emb_string& lhs, const emb_string& rhs) const {return lhs==rhs ;}

        };

}

#endif /* __SUPSTL_H__ */
