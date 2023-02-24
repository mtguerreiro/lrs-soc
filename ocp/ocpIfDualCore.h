/*
 * ocpIfDualCore.h
 *
 *  Created on: 24.02.2023
 *      Author: mguerreiro
 */

#ifndef OCP_IF_DUAL_CORE_H_
#define OCP_IF_DUAL_CORE_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpIf.h"

#include "ocpTrace.h"
#include "ocpCS.h"

#include "stddef.h"

#include "rp.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetNumberControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* OCP_IF_DUAL_CORE_H_ */
