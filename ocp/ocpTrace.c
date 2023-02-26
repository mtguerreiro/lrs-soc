/*
 * ocpTrace.c
 *
 *  Created on: 17.02.2023
 *      Author: mguerreiro
 */

//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "ocpTrace.h"
//===========================================================================

//===========================================================================
/*------------------------------ Definitions ------------------------------*/
//===========================================================================

typedef ctrace_t ocpTrace_t;

typedef struct ocpTraceControl_t{

	ocpTrace_t traces[OCP_TRACE_END];

	char names[OCP_TRACE_END * OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN];

	char *np;
}ocpTraceControl_t;
//===========================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpTraceControl_t xcontrol = {.np = xcontrol.names};

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpTraceInitialize(uint32_t id, ocpTraceConfig_t *config, char *name){

	char *tplim;

	if( id >= OCP_TRACE_END ) return -1;

	ctraceInitialize( &xcontrol.traces[id] , config );

	tplim = xcontrol.names + OCP_TRACE_END * OCP_TRACE_CONFIG_TRACE_NAME_MAX_LEN;
	while( *name && (xcontrol.np < tplim) ) *xcontrol.np++ = *name++;
	*xcontrol.np++ = 0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceAddSignal(uint32_t id, void *src, char *name){

	if( id >= OCP_TRACE_END ) return -1;

	ctraceAddSignal( &xcontrol.traces[id] , src, name );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetAddress(uint32_t id, void *address){

	if( id >= OCP_TRACE_END ) return -1;

	ctraceGetAddress( &xcontrol.traces[id], address );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceReset(uint32_t id){

	if( id >= OCP_TRACE_END ) return -1;

	ctraceReset( &xcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetSize(uint32_t id){

	if( id >= OCP_TRACE_END ) return -1;

	return ctraceGetSize( &xcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceSetSize(uint32_t id, int32_t size){

	if( id >= OCP_TRACE_END ) return -1;

	return ctraceSetSize( &xcontrol.traces[id], size );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetNumberSignals(uint32_t id){

	if( id >= OCP_TRACE_END ) return -1;

	return ctraceGetNumberSignals( &xcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetSignalsNames(uint32_t id, char *buffer, int32_t maxsize){

	if( id >= OCP_TRACE_END ) return -1;

	return ctraceGetSignalsNames( &xcontrol.traces[id], buffer, maxsize );
}
//---------------------------------------------------------------------------
int32_t ocpTraceGetNumberTraces(void){

	return OCP_TRACE_END;
}
//---------------------------------------------------------------------------
int32_t ocpTraceGetTracesNames(char *buffer, int32_t maxsize){

	int32_t k;
	char *p;

	p = xcontrol.names;
	k = 0;
	while( (p < xcontrol.np) && (k < maxsize) ){
		*buffer++ = *p++;
		k++;
	}

	return k;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceSave(uint32_t id){

	if( id >= OCP_TRACE_END ) return -1;

	ctraceSave( &xcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
