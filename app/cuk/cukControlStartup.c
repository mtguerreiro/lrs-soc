/*
 * cukControlStartup.c
 *
 *  Created on: 26.10.2023
 *      Author: marco
 */

#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "cukControlStartup.h"

#include "ocpConfig.h"
#include "ocpTrace.h"

#include "cukConfig.h"
//#include "config/stypesCuk.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static float u = 0.0f;
static float ufinal = 0.5f;
static float uinc = 50.0f / 100000.0f;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControlStartupInitialize(void){

}
//-----------------------------------------------------------------------------
int32_t cukControlStartupSetParams(void *params, uint32_t n){

    float *p = (float *)params;

    uinc = *p++;
    ufinal = *p++;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t cukControlStartupGetParams(void *in, uint32_t insize, void *out, uint32_t maxoutsize){

    float *p = (float *)out;

    *p++ = uinc;
    *p++ = ufinal;

    return 8;
}
//-----------------------------------------------------------------------------
int32_t cukControlStartupRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    u += uinc;

    if( u >= ufinal ) u = ufinal;
    if( u <= 0.0f ) u = 0.0f;

    o->u = u;

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
void cukControlStartupReset(void){

    u = 0.0f;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */