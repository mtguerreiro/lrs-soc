/*
 * boostControlDisabled.c
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "boostControlDisabled.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "boostConfig.h"
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
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControlDisabledInitialize(void){

}
//-----------------------------------------------------------------------------
int32_t boostControlDisabledSetParams(void *params, uint32_t n){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlDisabledGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t boostControlDisabledRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    boostConfigMeasurements_t *m = (boostConfigMeasurements_t *)meas;
    boostConfigControl_t *o = (boostConfigControl_t *)outputs;

    o->u = 0.0f;

    return sizeof(boostConfigControl_t);
}
//-----------------------------------------------------------------------------
void boostControlDisabledReset(void){

}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
