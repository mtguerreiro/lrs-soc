/*
 * ocpTrace.h
 *
 *  Created on: 17.02.2023
 *      Author: mguerreiro
 */

#ifndef OCPTRACE_H_
#define OCPTRACE_H_

//===========================================================================
/*------------------------------- Includes --------------------------------*/
//===========================================================================
#include "stdint.h"

#include "ctrace.h"
//===========================================================================

//===========================================================================
/*------------------------------ Definitions ------------------------------*/
//===========================================================================

typedef ctraceConfig_t ocpTraceConfig_t;

#define OCPTRACE_CONFIG_TRACE_NAME_LEN_MAX	20

typedef enum{
	OCPTRACE_1 = 0,
	OCPTRACE_2,
	OCPTRACE_END
}ocpTraceIDs_t;

//===========================================================================

//===========================================================================
/*------------------------------- Functions -------------------------------*/
//===========================================================================
//---------------------------------------------------------------------------
int32_t ocpTraceInitialize(uint32_t id, ocpTraceConfig_t *config, char *name);
//---------------------------------------------------------------------------
int32_t ocpTraceAddSignal(uint32_t id, void *src, char *name);
//---------------------------------------------------------------------------
int32_t ocpTraceGetAddress(uint32_t id, void *address);
//---------------------------------------------------------------------------
int32_t ocpTraceReset(uint32_t id);
//---------------------------------------------------------------------------
int32_t ocpTraceGetSize(uint32_t id);
//---------------------------------------------------------------------------
int32_t ocpTraceSetSize(uint32_t id, int32_t size);
//---------------------------------------------------------------------------
int32_t ocpTraceGetNumberSignals(uint32_t id);
//---------------------------------------------------------------------------
int32_t ocpTraceGetSignalsNames(uint32_t id, char *buffer);
//---------------------------------------------------------------------------
int32_t ocpTraceGetNumberTraces(void);
//---------------------------------------------------------------------------
int32_t ocpTraceGetTracesNames(char *buffer);
//---------------------------------------------------------------------------
int32_t ocpTraceSave(uint32_t id);
//---------------------------------------------------------------------------
//===========================================================================

#endif /* OCPTRACE_H_ */
