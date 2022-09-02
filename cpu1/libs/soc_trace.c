/*
 * soc_trace.c
 *
 *  Created on: 21 de ago de 2022
 *      Author: marco
 */

//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "stdint.h"

#include "soc_trace.h"
#include "soc_trace_mem.h"
//===========================================================================

//===========================================================================
/*------------------------------ Definitions ------------------------------*/
//===========================================================================
typedef struct soctrace_t{

	uint32_t n;
	uint32_t **data;
	uint8_t *dtype;
	char *tags;
	char *tp;
	soctracemem_t mem;
}soctrace_t;

soctrace_t xtraces[SOC_TRACE_ID_END];
//===========================================================================

//===========================================================================
/*------------------------------- Functions -------------------------------*/
//===========================================================================
//---------------------------------------------------------------------------
void soctraceInitialize(soctraceIDs_t id, uint32_t *mem, uint32_t size, uint32_t **data, uint8_t *dtype,  char *tags){

	soctracememInitialize( &xtraces[id].mem, mem, size );

	xtraces[id].data = data;
	xtraces[id].dtype = dtype;
	xtraces[id].tags = tags;
	xtraces[id].tp = tags;
}
//---------------------------------------------------------------------------
void soctraceAdd(soctraceIDs_t id, uint32_t *src, uint8_t dtype, char *tag){

	uint32_t n;

	n = xtraces[id].n;

	xtraces[id].data[n] = src;
	xtraces[id].dtype[n] = dtype;

	while(*tag){
		*(xtraces[id].tp++) = *tag++;
	}
	xtraces[id].tp++;

	xtraces[id].n++;
}
//---------------------------------------------------------------------------
void soctraceReset(soctraceIDs_t id){

	soctracememReset(&xtraces[id].mem);
}
//---------------------------------------------------------------------------
void soctraceSetSize(soctraceIDs_t id, uint32_t size){

	soctracememSetSize( &xtraces[id].mem, size );

}
//---------------------------------------------------------------------------
uint32_t soctraceReadQtyTraces(soctraceIDs_t id){

	return xtraces[id].n;
}
//---------------------------------------------------------------------------
uint32_t soctraceReadTags(soctraceIDs_t id, char *buffer){

	uint32_t k;
	char *p;

	p = xtraces[id].tags;
	while( p <= xtraces[id].tp ) *buffer++ = *p++;

	k = xtraces[id].tp - xtraces[id].tags;

	return k;
}
//---------------------------------------------------------------------------
uint32_t soctraceReadDTypes(soctraceIDs_t id, uint8_t *buffer){

	uint32_t k;
	uint8_t *p;

	p = xtraces[id].dtype;
	k = xtraces[id].n;
	while( k-- ) *buffer++ = *p++;

	return xtraces[id].n;
}
//---------------------------------------------------------------------------
void soctraceSave(soctraceIDs_t id){

	soctracememSave(&xtraces[id].mem, xtraces[id].data, xtraces[id].n);
}
//---------------------------------------------------------------------------
//===========================================================================
