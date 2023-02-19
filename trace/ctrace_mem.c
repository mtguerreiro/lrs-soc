/*
 * ctrace_mem.c
 *
 *  Created on: 21 de ago de 2022
 *      Author: marco
 */

//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "ctrace_mem.h"

//===========================================================================

//===========================================================================
/*------------------------------- Functions -------------------------------*/
//===========================================================================
//---------------------------------------------------------------------------
void ctracememInitialize(ctracemem_t *trace, void *mem, int32_t size){

	trace->start = mem;
	trace->end = mem + size;
	trace->p = mem;

	trace->maxsize = size;
}
//---------------------------------------------------------------------------
void ctracememAddress(ctracemem_t *trace, void *address){

	size_t *p = (size_t *)address;

	*p = (size_t)( trace->start );
}
//---------------------------------------------------------------------------
int32_t ctracememGetSize(ctracemem_t *trace){

	int32_t size;

	size = (trace->end - trace->start) * sizeof(size_t);

	return size;
}
//---------------------------------------------------------------------------
int32_t ctracememSetSize(ctracemem_t *trace, int32_t size){

	if( size > trace->maxsize ) return -1;

	size = size / sizeof(size_t);

	trace->end = trace->start + size;
	trace->p = trace->start;

	return 0;
}
//---------------------------------------------------------------------------
void ctracememReset(ctracemem_t *trace){

	trace->p = trace->start;
}
//---------------------------------------------------------------------------
void ctracememSave(ctracemem_t *trace, void **src, uint32_t size){

	size_t **s = (size_t **)src;

	size = size / sizeof(size_t);

	if( (trace->p + size) > trace->end ) return;

	while(size--) *trace->p++ = **s++;
}
//---------------------------------------------------------------------------
//===========================================================================
