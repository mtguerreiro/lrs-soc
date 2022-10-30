/*
 * controlsysHwIf.c
 *
 *  Created on: 4 de set de 2022
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "controlsysHwIf.h"

//#include "buckHwIf.h"
#include "buckHwIfPlecs.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t controlsysHwIfInitialize(void){

	return buckHwIfPlecsInitialize();
}
//-----------------------------------------------------------------------------
int32_t controlsysHwIfGetMeas(void *meas){

	return buckHwIfPlecsGetMeas( meas );
}
//-----------------------------------------------------------------------------
void controlsysHwIfProcMeas(void *meas, void *procmeas, int32_t n){

	buckHwIfPlecsProcMeas( meas, procmeas, n );
}
//-----------------------------------------------------------------------------
void controlsysHwIfProcOutputs(void *outputs, void *procoutputs, int32_t n){

	buckHwIfPlecsProcOutputs( outputs, procoutputs, n );
}
//-----------------------------------------------------------------------------
void controlsysHwIfApplyOutputs(void *outputs, int32_t n){

	buckHwIfPlecsApplyOutputs( outputs, n );
}
//-----------------------------------------------------------------------------
int32_t controlsysHwIfInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

	return buckHwIfPlecsInterface(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
//=============================================================================
