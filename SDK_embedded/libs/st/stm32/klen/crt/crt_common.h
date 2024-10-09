#ifndef __CRT_COMMON_H__
#define __CRT_COMMON_H__


#if (USE_RAM_VEC_TABLE == 1)
//-------------------------------------------------------------------
void vec_map2ram ( unsigned* vec_table );
void vec_map2flash ( unsigned* vec_table );

void vec_set (  VectorType vec_type  , void* handler ) ;
void vec_table_copy2ram(unsigned map_needed ) ;

#endif /*RAM_VEC_TABLE*/

#endif /*__CRT_COMMON_H__*/
