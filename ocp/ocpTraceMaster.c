/*
 * ocpTraceMaster.c
 *
 *  Created on: 23.03.2023
 *      Author: mguerreiro
 */

//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "ocpTraceMaster.h"

#include "ocpConfig.h"
//===========================================================================

//===========================================================================
/*------------------------------ Definitions ------------------------------*/
//===========================================================================

typedef ctrace_t ocpTraceMaster_t;

typedef struct ocpTraceMasterControl_t{

	ocpTraceMaster_t traces[OCP_TRACE_MASTER_END];

	char names[OCP_TRACE_MASTER_END * OCP_TRACE_MASTER_CONFIG_TRACE_NAME_MAX_LEN];

	char *np;
}ocpTraceMasterControl_t;
//===========================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpTraceMasterControl_t xifcontrol = {.np = xifcontrol.names};

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterInitialize(uint32_t id, ocpTraceMasterConfig_t *config, char *name){

	char *tplim;

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	ctraceInitialize( &xifcontrol.traces[id] , config );

	tplim = xifcontrol.names + OCP_TRACE_MASTER_END * OCP_TRACE_MASTER_CONFIG_TRACE_NAME_MAX_LEN;
	while( *name && (xifcontrol.np < tplim) ) *xifcontrol.np++ = *name++;
	*xifcontrol.np++ = 0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterAddSignal(uint32_t id, void *src, char *name){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	ctraceAddSignal( &xifcontrol.traces[id] , src, name );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterGetAddress(uint32_t id, void *address){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	ctraceGetAddress( &xifcontrol.traces[id], address );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterReset(uint32_t id){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	ctraceReset( &xifcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterGetSize(uint32_t id){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	return ctraceGetSize( &xifcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterSetSize(uint32_t id, int32_t size){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	return ctraceSetSize( &xifcontrol.traces[id], size );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterGetNumberSignals(uint32_t id){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	return ctraceGetNumberSignals( &xifcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterGetSignalsNames(uint32_t id, char *buffer, int32_t maxsize){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	return ctraceGetSignalsNames( &xifcontrol.traces[id], buffer, maxsize );
}
//---------------------------------------------------------------------------
int32_t ocpTraceMasterGetNumberTraces(void){

	return OCP_TRACE_MASTER_END;
}
//---------------------------------------------------------------------------
int32_t ocpTraceMasterGetTracesNames(char *buffer, int32_t maxsize){

	int32_t k;
	char *p;

	p = xifcontrol.names;
	k = 0;
	while( (p < xifcontrol.np) && (k < maxsize) ){
		*buffer++ = *p++;
		k++;
	}

	return k;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceMasterSave(uint32_t id){

	if( id >= OCP_TRACE_MASTER_END ) return -1;

	ctraceSave( &xifcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
