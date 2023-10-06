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
static uint32_t counter = 0;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControlOLInitialize(void){

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
int32_t cukControlOLRun(void *meas, int32_t nmeas, void *refs, int32_t nrefs, void *outputs, int32_t nmaxoutputs){

    cukConfigMeasurements_t *m = (cukConfigMeasurements_t *)meas;
    cukConfigReferences_t *r = (cukConfigReferences_t *)refs;
    cukConfigControl_t *o = (cukConfigControl_t *)outputs;

    u = 1.0f - 16.0f / r->v_o;

    if( u > 1.0f ) u = 1.0f;
    if( u < 0.0f ) u = 0.0f;

    o->u = u;

//    u = u + 0.545455f / (625.0f);
//    if( u > 0.545455f ) u = 0.545455f;
//
//    o->u = u;
//
//    counter++;
//    if( counter > 800 ){
//        o->sw_o = 1;
//        counter = 802;
//    }
//    else{
//        o->sw_o = 0;
//    }

    return sizeof(cukConfigControl_t);
}
//-----------------------------------------------------------------------------
void cukControlOLReset(void){

    u = 0.0f;
    counter = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
#endif /* SOC_CPU1 */
