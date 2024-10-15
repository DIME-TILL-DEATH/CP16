#ifndef __INIT_H__
#define __INIT_H__


void init(void);

inline void __attribute__ ((always_inline)) delay_nop(uint32_t i)
{
	for (uint32_t ii = 0 ; ii < i ; ii++) NOP();
}

static inline uint32_t __attribute__ ((always_inline)) high_sort(uint32_t* p, size_t n)
{
	uint32_t temp = 0;
	for(uint16_t i = 0 ; i < n ; i++)
	{
		if(temp < p[i])temp = p[i];
	}
	return temp;
}
#endif /*__INIT_H__*/
uint32_t sw1_state() ;
