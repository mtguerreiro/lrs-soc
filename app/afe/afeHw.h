/*
 * @file afeHw.h
 *
 */

#ifndef AFE_HW_H_
#define AFE_HW_H_

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
#include "stddef.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t afeHwInitialize(void *initparams);
//-----------------------------------------------------------------------------
int32_t afeHwAdcEn(uint32_t enable);
//-----------------------------------------------------------------------------
int32_t afeHwSetAdc(void *adcparams);
//-----------------------------------------------------------------------------
int32_t afeHwSetPwm(void *pwmparams);
//-----------------------------------------------------------------------------
void afeHwSetInputRelay(uint32_t state);
//-----------------------------------------------------------------------------
void afeHwSetOutputRelay(uint32_t state);
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */

#endif /* AFE_HW_H_ */
