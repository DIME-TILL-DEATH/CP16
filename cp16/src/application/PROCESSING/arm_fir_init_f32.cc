/*-----------------------------------------------------------------------------    
 * Copyright (C) 2010-2013 ARM Limited. All rights reserved.    
 *    
 * $Date:        17. January 2013
 * $Revision: 	V1.4.1
 *    
 * Project: 	    CMSIS DSP Library    
 * Title:        arm_fir_init_f32.c    
 *    
 * Description:  Floating-point FIR filter initialization function.    
 *    
 * Target Processor: Cortex-M4/Cortex-M3/Cortex-M0
 *  
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions
 * are met:
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   - Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the 
 *     distribution.
 *   - Neither the name of ARM LIMITED nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE 
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.   
 * ---------------------------------------------------------------------------*/

#include "arm_math.h"

/**    
 * @ingroup groupFilters    
 */

/**    
 * @addtogroup FIR    
 * @{    
 */

/**    
 * @details    
 *    
 * @param[in,out] *S points to an instance of the floating-point FIR filter structure.    
 * @param[in] 	  numTaps  Number of filter coefficients in the filter.    
 * @param[in]     *pCoeffs points to the filter coefficients buffer.    
 * @param[in]     *pState points to the state buffer.    
 * @param[in] 	  blockSize number of samples that are processed per call.    
 * @return 		  none.    
 *    
 * <b>Description:</b>    
 * \par    
 * <code>pCoeffs</code> points to the array of filter coefficients stored in time reversed order:    
 * <pre>    
 *    {b[numTaps-1], b[numTaps-2], b[N-2], ..., b[1], b[0]}    
 * </pre>    
 * \par    
 * <code>pState</code> points to the array of state variables.    
 * <code>pState</code> is of length <code>numTaps+blockSize-1</code> samples, where <code>blockSize</code> is the number of input samples processed by each call to <code>arm_fir_f32()</code>.    
 */

void arm_fir_init_f32(arm_fir_instance_f32 *S, uint16_t numTaps,
		float32_t *pCoeffs, float32_t *pState, uint32_t blockSize) {
	/* Assign filter taps */
	S->numTaps = numTaps;

	/* Assign coefficient pointer */
	S->pCoeffs = pCoeffs;

	/* Clear state buffer and the size of state buffer is (blockSize + numTaps - 1) */
	memset(pState, 0, (numTaps + (blockSize - 1u)) * sizeof(float32_t));

	/* Assign state pointer */
	S->pState = pState;

}

/**    
 * @} end of FIR group    
 */
