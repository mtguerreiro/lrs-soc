/*
 * @file afe.c
 *
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "afe.h"

#include "afeIf.h"

#include "afeHw.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t afeInitialize(void *initparams){

	afeIfInitialize();

	return afeHwInitialize(initparams);
}
//-----------------------------------------------------------------------------
int32_t afeAdcEn(uint32_t enable){

	afeHwAdcEn(enable);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeSetAdc(void *adcparams){

	afeHwSetAdc(adcparams);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t afeSetPwm(void *pwmparams){

	afeHwSetPwm(pwmparams);

	return 0;
}
//-----------------------------------------------------------------------------
void afeSetInputRelay(uint32_t state){

	afeHwSetInputRelay(state);
}
//-----------------------------------------------------------------------------
void afeSetOutputRelay(uint32_t state){

	afeHwSetOutputRelay(state);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
