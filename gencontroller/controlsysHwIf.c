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

#include "buckHwIf.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t controlsysHwIfInitialize(void){

	return 0;
}
//-----------------------------------------------------------------------------
int32_t controlsysHwIfGetMeas(void *meas){

	return buckHwIfGetMeas( meas );
}
//-----------------------------------------------------------------------------
void controlsysHwIfProcMeas(void *meas, void *procmeas, int32_t n){

	buckHwIfProcMeas( meas, procmeas, n );
}
//-----------------------------------------------------------------------------
void controlsysHwIfProcOutputs(void *outputs, void *procoutputs, int32_t n){

	buckHwIfProcOutputs( outputs, procoutputs, n );
}
//-----------------------------------------------------------------------------
void controlsysHwIfApplyOutputs(void *outputs, int32_t n){

	buckHwIfApplyOutputs( outputs, n );
}
//-----------------------------------------------------------------------------
int32_t controlsysHwIfSetParams(void *params, int32_t n){

	return buckHwIfSetParams( params, n );
}
//-----------------------------------------------------------------------------
int32_t controlsysHwIfGetParams(void *in, void *out){

	return buckHwIfGetParams( in, out );
}
//-----------------------------------------------------------------------------
//=============================================================================
