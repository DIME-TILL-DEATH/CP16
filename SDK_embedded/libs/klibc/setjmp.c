#include <setjmp.h>

 void	__attribute__ (( naked )) longjmp(jmp_buf jmpb, int retval)
{

  	/* If we have stack extension code it ought to be handled here.  */

  	/* Restore the registers, retrieving the state when setjmp() was called.  */
  #ifdef __thumb2__
  	asm volatile ("ldmfd a1!, { v1-v7, fp, ip, lr }");
  	asm volatile ("mov sp, ip");
  #else
  	asm volatile ("ldmfd a1!, { v1-v7, fp, ip, sp, lr }");
  #endif

  	/* Put the return value into the integer result register.
  	   But if it is zero then return 1 instead.  */
  	asm volatile ("movs a1, a2");
  #ifdef __thumb2__
  	asm volatile ("it eq");
  #endif
  	asm volatile ("moveq a1, #1");

  #ifdef __APCS_26__
	asm volatile ("movs pc, lr");
  #elif defined(__thumb2__)
	asm volatile ("bx lr");
  #else
	asm volatile ("tst		lr, #1");
	asm volatile ("moveq		pc, lr");
	asm volatile (".word           0xe12fff1e	/* bx lr */");
  #endif
}

 int __attribute__ (( naked )) setjmp(jmp_buf jmpb)
{
	//FUNC_START setjmp

	/* Save all the callee-preserved registers into the jump buffer.  */
#ifdef __thumb2__
	asm volatile ("mov ip, sp") ;
	asm volatile ("stmea a1!, { v1-v7, fp, ip, lr }") ;
#else
	asm volatile ("stmea a1!, { v1-v7, fp, ip, sp, lr }");
#endif

	/* When setting up the jump buffer return 0.  */
	asm volatile ("mov a1, #0");

#ifdef __APCS_26__
	asm volatile ("movs pc, lr");
#elif defined(__thumb2__)
	asm volatile ("bx lr");
#else
	asm volatile ("tst		lr, #1");
	asm volatile ("moveq		pc, lr");
	asm volatile (".word           0xe12fff1e	/* bx lr */");
#endif
}

