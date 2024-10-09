/*
 * mmgr.h
 *
 *  Created on: 15.09.2012
 *      Author: klen
 */

#ifndef MMGR_H_
#define MMGR_H_

#ifdef __cplusplus
extern "C" {
#endif

  void  heap_init();
  void  heap_deinit();

  size_t heap_block_size(void* ptr);
  size_t heap_control_stuct_size();
  size_t heap_align_size();
  size_t heap_block_size_min();
  size_t heap_block_size_max();
  size_t heap_pool_overhead();
  size_t heap_alloc_overhead();

  void*  heap_ptr();

void*  malloc(size_t size);
void*  realloc(void *ptr, size_t size);
void*  calloc(size_t nelem, size_t elem_size);
void   free(void *ptr);


void heap_alloc_free_counter_reset();
void heap_alloc_free_counter( size_t* a, size_t* f );


#ifdef __cplusplus
  }
#endif

#endif /* MMGR_H_ */
