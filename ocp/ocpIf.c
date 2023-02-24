/*
 * ocpIf.c
 *
 *  Created on: 18 de fev de 2023
 *      Author: marco
 */

#define OCP_IF_CONFIG_DUAL_CORE
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpIf.h"

#include "ocpTrace.h"
#include "ocpCS.h"

#include "stddef.h"

#include "rp.h"

#ifdef OCP_IF_CONFIG_DUAL_CORE
#include "ipcClient.h"
#endif
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//#ifndef OCP_IF_CONFIG_DUAL_CORE
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//#else
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
//#endif
//=============================================================================

//=============================================================================
/*-------------------------------- Definitions ------------------------------*/
//=============================================================================

typedef struct{

	rphandle_t handles[OCP_IF_CMD_END];
	rpctx_t rp;

}ocpIfControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpIfControl_t xcontrol;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpIfInitialize(void){

	rpInitialize( &xcontrol.rp, OCP_IF_CMD_END, xcontrol.handles );

//#ifndef OCP_IF_CONFIG_DUAL_CORE
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_READ, ocpIfTraceRead );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_RESET, ocpIfTraceReset );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIZE, ocpIfTraceGetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_SET_SIZE, ocpIfTraceSetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, ocpIfTraceGetNumberSignals );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, ocpIfTraceGetSignalsNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, ocpIfTraceGetNumberTraces );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_TRACES_NAMES, ocpIfTraceGetTracesNames );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_STATUS, ocpIfCSStatus );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_ENABLE, ocpIfCSEnable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_DISABLE, ocpIfCSDisable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_CONTROLLER_IF, ocpIfCSControllerIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_HARDWARE_IF, ocpIfCSHardwareIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, ocpIfCSGetNumberControllers );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, ocpIfCSGetNumberControllersNames );
//#else
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_READ, ocpIfDualCoreTraceRead );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_RESET, ocpIfDualCoreTraceReset );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_GET_SIZE, ocpIfDualCoreTraceGetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_SET_SIZE, ocpIfDualCoreTraceSetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_GET_NUMBER_SIGNALS, ocpIfDualCoreTraceGetNumberSignals );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_GET_SIGNALS_NAMES, ocpIfDualCoreTraceGetSignalsNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_GET_NUMBER_TRACES, ocpIfDualCoreTraceGetNumberTraces );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_TRACE_GET_TRACES_NAMES, ocpIfDualCoreTraceGetTracesNames );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_STATUS, ocpIfDualCoreCSStatus );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_ENABLE, ocpIfDualCoreCSEnable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_DISABLE, ocpIfDualCoreCSDisable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_CONTROLLER_IF, ocpIfDualCoreCSControllerIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_HARDWARE_IF, ocpIfDualCoreCSHardwareIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_GET_NUMBER_CONTROLLERS, ocpIfDualCoreCSGetNumberControllers );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_DUAL_CORE_CMD_CS_GET_CONTROLLERS_NAMES, ocpIfDualCoreCSGetNumberControllersNames );
//#endif

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpIf(void *in, int32_t insize, void **out, int32_t maxoutsize){

	return rpRequest( &xcontrol.rp, in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//#ifndef OCP_IF_CONFIG_DUAL_CORE
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	size_t address;
	uint32_t size;
	int32_t status;

	id = *( (uint32_t *)in );

	status = ocpTraceGetAddress( id, (void *)(&address) );
	if( status < 0 ) return -1;

	size = ocpTraceGetSize( id );
	if( size < 0 ) return -1;

	*out = (void *)( address );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;

	id = *( (uint32_t *)in );

	status = ocpTraceReset(id);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t size;
	uint32_t *o = (uint32_t *)*out;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	size = ocpTraceGetSize(id);

	*o = size;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t size;
	int32_t status;
	uint32_t *p = (uint32_t *)in;

	id = *p++;
	size = *p;

	status = ocpTraceSetSize(id, size);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t n;

	uint32_t *o = (uint32_t *)( *out );

	id = *( (uint32_t *)in );

	n = ocpTraceGetNumberSignals(id);

	*o = n;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t size;

	char *o = (char *)( *out );

	id = *( (uint32_t *)in );

	size = ocpTraceGetSignalsNames(id, o);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t n;

	uint32_t *o = (uint32_t *)( *out );

	n = ocpTraceGetNumberTraces();

	*o = n;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpTraceGetTracesNames(o);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;

	uint32_t *o = (uint32_t *)( *out );

	id = *( (uint32_t *)in );

	status = ocpCSStatus(id);

	*o = status;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;

	id = *( (uint32_t *)in );

	status = ocpCSEnable(id);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;

	id = *( (uint32_t *)in );

	status = ocpCSDisable(id);

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	return ocpCSControllerInterface(id, (void *)p, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	return ocpCSHardwareInterface(id, (void *)p, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t n;

	uint32_t *o = (uint32_t *)( *out );

	n = ocpCSGetNumberControllers();

	*o = n;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpCSGetControllersNames(o);

	return size;
}
//#else
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;

	uint32_t nTracesC1;
	uint32_t *p;
	uint32_t cmd;

	id = *( (uint32_t *)in );

	cmd = OCP_IF_CMD_TRACE_GET_NUMBER_TRACES;
	p = &nTracesC1;
	status = ipcClientRequest((void *)cmd, 4, (void **)&p, 4, 100);
	if( status < 0 ) return status;

	if( id < nTracesC1){
		cmd = OCP_IF_CMD_TRACE_RESET;
		status = ipcClientRequest((void *)cmd, 4, (void **)&p, 4, 100);
	}
	else{
		status = ocpTraceReset(id);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetNumberControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

}
//-----------------------------------------------------------------------------
//#endif
//=============================================================================
