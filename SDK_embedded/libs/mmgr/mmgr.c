#include "tlsf.h"
#include "mmgr.h"
#include <string.h>

#define heap_lock() {}
#define heap_unlock() {}

static tlsf_t tlsf = NULL ;

// ---------------    POOLS DESCRIPTIONS ------------------
#if defined (INTERNAL_SRAM_POOL_SIZE)
     static char  internal_sram_pool[INTERNAL_SRAM_POOL_SIZE] ;
#endif

#if defined (EXT_MEM_BANK0_POOL_SIZE)
   __attribute__ ((section(".ext_mem_bank0_bss")))  static char ext_mem_bank0_pool[EXT_MEM_BANK0_POOL_SIZE] ;
#endif

#if defined (EXT_MEM_BANK1_POOL_SIZE)
   __attribute__ ((section(".ext_mem_bank1_bss")))  static char ext_mem_bank1_pool[EXT_MEM_BANK1_POOL_SIZE] ;
#endif

#if defined (EXT_MEM_BANK2_POOL_SIZE)
   __attribute__ ((section(".ext_mem_bank2_bss")))  static char ext_mem_bank2_pool[EXT_MEM_BANK2_POOL_SIZE] ;
#endif

#if defined (EXT_MEM_BANK3_POOL_SIZE)
   __attribute__ ((section(".ext_mem_bank3_bss")))  static char ext_mem_bank3_pool[EXT_MEM_BANK3_POOL_SIZE] ;
#endif

   typedef enum {
                    // if not user defined internal sram area, used a default  INTERNAL_SRAM_POOL_SIZE = 0 heap dummy
                    #if defined (INTERNAL_SRAM_POOL_SIZE)
                        hpiInternalSram,
                    #endif

                    #if defined (EXT_MEM_BANK0_POOL_SIZE)
   		     hpiExtMemBank0,
                    #endif

                    #if defined (EXT_MEM_BANK1_POOL_SIZE)
   		     hpiExtMemBank1,
                    #endif

                    #if defined (EXT_MEM_BANK2_POOL_SIZE)
   		     hpiExtMemBank2,
                    #endif

                    #if defined (EXT_MEM_BANK3_POOL_SIZE)
   		     hpiExtMemBank3,
                    #endif

   		 hpiHeapPoolsCount
                } heap_pool_id_t ;

void heap_init()
        {
                    #if defined (INTERNAL_SRAM_POOL_SIZE)
                        tlsf = tlsf_create_with_pool(internal_sram_pool, INTERNAL_SRAM_POOL_SIZE);
                        if (!tlsf)
                           while(1)
                              asm volatile ("nop");
                    #endif

                    #if defined (EXT_MEM_BANK0_POOL_SIZE)
                        if(tlsf)
                          {
                            pool_t pool = tlsf_add_pool(tlsf, ext_mem_bank0_pool, EXT_MEM_BANK0_POOL_SIZE);
                          }
                        else
                          {
                            tlsf = tlsf_create_with_pool(ext_mem_bank0_pool, EXT_MEM_BANK0_POOL_SIZE);
                          }
                    #endif

                    #if defined (EXT_MEM_BANK1_POOL_SIZE)
                        if(tlsf)
                          {
                             pool_t pool = tlsf_add_pool(tlsf, ext_mem_bank1_pool, EXT_MEM_BANK1_POOL_SIZE);
                          }
                        else
                          {
                             tlsf = tlsf_create_with_pool(ext_mem_bank1_pool, EXT_MEM_BANK1_POOL_SIZE);
                          }
                   #endif

                   #if defined (EXT_MEM_BANK2_POOL_SIZE)
                        if(tlsf)
                          {
                              pool_t pool = tlsf_add_pool(tlsf, ext_mem_bank2_pool, EXT_MEM_BANK2_POOL_SIZE);
                          }
                        else
                          {
                              tlsf = tlsf_create_with_pool(ext_mem_bank2_pool, EXT_MEM_BANK2_POOL_SIZE);
                          }
                   #endif

                   #if defined (EXT_MEM_BANK3_POOL_SIZE)
                        if(tlsf)
                          {
                              pool_t pool = tlsf_add_pool(tlsf, ext_mem_bank3_pool, EXT_MEM_BANK3_POOL_SIZE);
                          }
                        else
                          {
                              tlsf = tlsf_create_with_pool(ext_mem_bank3_pool, EXT_MEM_BANK3_POOL_SIZE);
                          }
                   #endif
        }


void heap_deinit()
        {
            heap_lock();
            tlsf_destroy(tlsf) ;
            heap_unlock();
        }

void*  heap_ptr()
{
    return  tlsf ;
}

/*
static struct
{
  size_t al ;
  size_t fr  ;
}  afc = {0,0} ;

void heap_alloc_free_count_reset() { afc.al = afc.fr = 0 ; }
void heap_alloc_free_count( size_t* a, size_t* f ) { *a=afc.al ;  *f = afc.fr ; }
*/

void* malloc(size_t size)
        {
           void* ptr ;
           heap_lock();
           ptr = tlsf_malloc(tlsf , size);
           heap_unlock();

           while(!ptr)
           {
        	   asm volatile ("nop");
        	   asm volatile ("nop");
        	   asm volatile ("nop");
           }

//           afc.al ++ ;

           return ptr ;
        }

void* calloc(size_t nelem, size_t elem_size)
        {
           void* ptr ;
           heap_lock();
           ptr = tlsf_malloc(tlsf , nelem * elem_size);
           heap_unlock();

           //           afc.fr ++ ;

           return ptr ;
        }

void* realloc(void *ptr, size_t size)
        {
           void* new_ptr ;
           heap_lock();
           new_ptr = tlsf_realloc(tlsf , ptr, size);
           heap_unlock();

           //           afc.fr ++ ;

           return new_ptr ;
        }

void free(void *ptr)
        {
           heap_lock();
           tlsf_free( tlsf , ptr );
           heap_unlock();

//           afc.fr ++ ;
        }



inline size_t heap_block_size(void* ptr)
{
  size_t size ;
  heap_lock();
  size = tlsf_block_size(ptr) ;
  heap_unlock();
  return size ;
}

inline size_t heap_control_stuct_size()
{
  size_t size ;
  heap_lock();
  size = tlsf_size() ;
  heap_unlock();
  return size ;
}

inline size_t heap_align_size()
{
  size_t size ;
  heap_lock();
  size = tlsf_align_size() ;
  heap_unlock();
  return size ;
}

inline size_t heap_block_size_min()
{
  size_t size ;
  heap_lock();
  size = tlsf_block_size_min() ;
  heap_unlock();
  return size ;
}

inline size_t heap_block_size_max()
{
  size_t size ;
  heap_lock();
  size = tlsf_block_size_max() ;
  heap_unlock();
  return size ;
}

inline size_t heap_pool_overhead()
{
  size_t overhead ;
  heap_lock();
  overhead = tlsf_pool_overhead() ;
  heap_unlock();
  return overhead ;
}

inline size_t heap_alloc_overhead()
{
  size_t overhead ;
  heap_lock();
  overhead = tlsf_pool_overhead() ;
  heap_unlock();
  return overhead ;
}


inline void* pvPortMalloc( size_t size ) __attribute__ ((alias ("malloc"))) ;
inline void vPortFree( void* pv ) __attribute__ ((alias ("free")));
