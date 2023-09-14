/*
 * cukControlOL.c
 *
 *  Created on: 11.09.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlOL.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "config/stypesCuk.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControlOLInitialize(void){

	ocpTraceAddSignal(OCP_TRACE_1, (void *)&u, "Control signal");
}
//-----------------------------------------------------------------------------
int32_t cukControlOLSetParams(void *params, uint32_t n){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlOLGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlOLRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs){

    stypesMeasurements_t *m = (stypesMeasurements_t *)inputs;
    stypesControl_t *o = (stypesControl_t *)outputs;

    u = u + 0.5f / (1000.0f);
    if( u > 0.5f ) u = 0.5f;

    o->d = u;

    return sizeof(stypesControl_t);
}
//-----------------------------------------------------------------------------
void cukControlOLReset(void){

    u = 0.0f;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
