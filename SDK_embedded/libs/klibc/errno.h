#ifndef __ERRNO__
#define __ERRNO__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __error_t_defined
typedef int error_t;
#define __error_t_defined 1
#endif

int* __errno(void);

#define errno *(__errno())

#ifdef __cplusplus
            }
#endif

#endif /*__ERRNO__*/



