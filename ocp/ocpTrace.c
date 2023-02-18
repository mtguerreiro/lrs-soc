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

	ocpTrace_t traces[OCPTRACE_END];

	char names[OCPTRACE_END * OCPTRACE_CONFIG_TRACE_NAME_LEN_MAX];

	char *np;
}ocpTraceControl_t;
//===========================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

ocpTraceControl_t xcontrol = {.np = xcontrol.names};

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpTraceInitialize(uint32_t id, ocpTraceConfig_t *config, char *name){

	char *tplim;

	if( id > OCPTRACE_END ) return -1;

	ctraceInitialize( &xcontrol.traces[id] , config );

	tplim = xcontrol.names + OCPTRACE_END * OCPTRACE_CONFIG_TRACE_NAME_LEN_MAX;
	while( *name && (xcontrol.np < tplim) ) *xcontrol.np++ = *name++;
	*xcontrol.np++ = 0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceAddSignal(uint32_t id, void *src, char *name){

	if( id > OCPTRACE_END ) return -1;

	ctraceAddSignal( &xcontrol.traces[id] , src, name );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetAddress(uint32_t id, void *address){

	if( id > OCPTRACE_END ) return -1;

	ctraceGetAddress( &xcontrol.traces[id], address );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceReset(uint32_t id){

	if( id > OCPTRACE_END ) return -1;

	ctraceReset( &xcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetSize(uint32_t id){

	if( id > OCPTRACE_END ) return -1;

	return ctraceGetSize( &xcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceSetSize(uint32_t id, int32_t size){

	if( id > OCPTRACE_END ) return -1;

	return ctraceSetSize( &xcontrol.traces[id], size );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetNumberSignals(uint32_t id){

	if( id > OCPTRACE_END ) return -1;

	return ctraceGetNumberSignals( &xcontrol.traces[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpTraceGetSignalsNames(uint32_t id, char *buffer){

	if( id > OCPTRACE_END ) return -1;

	return ctraceGetSignalsNames( &xcontrol.traces[id], buffer );
}
//---------------------------------------------------------------------------
int32_t ocpTraceGetNumberTraces(void){

	return OCPTRACE_END;
}
//---------------------------------------------------------------------------
int32_t ocpTraceGetTracesNames(char *buffer){

	int32_t k;
	char *p;

	p = xcontrol.names;
	while( p <= xcontrol.np ) *buffer++ = *p++;

	k = xcontrol.np - xcontrol.names;

	return k;
}
//-----------------------------------------------------------------------------
int32_t ocpTraceSave(uint32_t id){

	if( id > OCPTRACE_END ) return -1;

	ctraceSave( &xcontrol.traces[id] );

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
