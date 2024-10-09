#ifndef __PRINTF_H__
#define __PRINTF_H__

#ifdef __cplusplus
extern "C" {
#endif

  typedef int (*send_char_fnc_t)(int c);

  int kprintf(send_char_fnc_t send_char ,const char *format, ...);
  int ksprintf(char *out, const char *format, ...);
  int kprint(send_char_fnc_t send_char , char **out, const char *format, int* args ) ;

 /* inline void ksprintfv( send_char_fnc_t send_char ,char* buff, const char** format )
  {
    kprint( &buff , *format, (int*)format + 1);
  }

  inline void kprintfv( send_char_fnc_t send_char ,const char** format )
  {
    kprint( 0 , *format, (int*)format + 1);
  }
*/

#ifdef __cplusplus
           }
#endif

#endif/*__PRINTF_H__*/
