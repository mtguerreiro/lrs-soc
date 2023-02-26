/*
 * ctrace.c
 *
 *  Created on: 21 de ago de 2022
 *      Author: marco
 */

/*
 * TODO: limit signal name
 */
//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "ctrace.h"
//===========================================================================

//===========================================================================
/*------------------------------ Definitions ------------------------------*/
//===========================================================================

//===========================================================================

//===========================================================================
/*------------------------------- Functions -------------------------------*/
//===========================================================================
//---------------------------------------------------------------------------
void ctraceInitialize(ctrace_t *trace, ctraceConfig_t *config){

	ctracememInitialize( &trace->mem, config->mem, config->size );

	trace->data = config->data;
	trace->names = config->names;
	trace->np = config->names;

	trace->n = 0;
}
//---------------------------------------------------------------------------
void ctraceAddSignal(ctrace_t *trace, void *src, char *name){

	uint32_t n;

	n = trace->n;

	trace->data[n] = src;

	while( *name ) *(trace->np++) = *name++;
	*(trace->np++) = 0;

	trace->n++;
}
//---------------------------------------------------------------------------
void ctraceGetAddress(ctrace_t *trace, void *address){

	ctracememAddress( &trace->mem, address );
}
//---------------------------------------------------------------------------
void ctraceReset(ctrace_t *trace){

	ctracememReset( &trace->mem );
}
//---------------------------------------------------------------------------
uint32_t ctraceGetSize(ctrace_t *trace){

	return ctracememGetSize( &trace->mem );
}
//---------------------------------------------------------------------------
int32_t ctraceSetSize(ctrace_t *trace, int32_t size){

	return ctracememSetSize( &trace->mem, size );
}
//---------------------------------------------------------------------------
int32_t ctraceGetNumberSignals(ctrace_t *trace){

	return trace->n;
}
//---------------------------------------------------------------------------
int32_t ctraceGetSignalsNames(ctrace_t *trace, char *buffer, int32_t maxsize){

	int32_t k;
	char *p;

	p = trace->names;

	k = 0;
	while( (p < trace->np) && (k < maxsize) ){
		*buffer++ = *p++;
		k++;
	}

	return k;
}
//---------------------------------------------------------------------------
void ctraceSave(ctrace_t *trace){

	uint32_t size;

	size = sizeof(size_t) * trace->n;

	ctracememSave( &trace->mem, trace->data, size );
}
//---------------------------------------------------------------------------
//===========================================================================
