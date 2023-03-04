/*
 * ocpIf.c
 *
 *  Created on: 18 de fev de 2023
 *      Author: marco
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpIf.h"

#include "ocpTrace.h"
#include "ocpCS.h"
#include "ocpPlatform.h"

#include "stddef.h"

#include "rp.h"

#if( OCP_IF_CONFIG_CORE == 1 )
#include "ipcClient.h"
#endif
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
#if( OCP_IF_CONFIG_CORE == 0 )
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
static int32_t ocpIfTraceGetAddress(void *in, uint32_t insize,
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
static int32_t ocpIfCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
#else
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
static int32_t ocpIfDualCoreCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberTracesSecondCore(void);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceResetSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSizeSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceSetSizeSecondCore(uint32_t id, int32_t size);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberSignalsSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSignalsNamesSecondCore(uint32_t id,
		char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetTracesNamesSecondCore(char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetAddressSecondCore(uint32_t id, void *address);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllersSecondCore(void);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSStatusSecondCore(uint32_t id, int32_t *status);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSEnableSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSDisableSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetControllersNamesSecondCore(char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSControllerInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfCSHardwareInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
static int32_t ocpIfPlatformID(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfPlatformIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Definitions ------------------------------*/
//=============================================================================

typedef struct{

	rphandle_t handles[OCP_IF_CMD_END];
	rpctx_t rp;

}ocpIfControl_t;

#define OCP_IF_CONFIG_DUAL_CORE_COMM_TO		100
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

#if( OCP_IF_CONFIG_CORE == 0 )
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_READ, ocpIfTraceRead );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_RESET, ocpIfTraceReset );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIZE, ocpIfTraceGetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_SET_SIZE, ocpIfTraceSetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, ocpIfTraceGetNumberSignals );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, ocpIfTraceGetSignalsNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, ocpIfTraceGetNumberTraces );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_TRACES_NAMES, ocpIfTraceGetTracesNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_ADDRESS, ocpIfTraceGetAddress );


	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_STATUS, ocpIfCSStatus );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_ENABLE, ocpIfCSEnable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_DISABLE, ocpIfCSDisable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_CONTROLLER_IF, ocpIfCSControllerIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_HARDWARE_IF, ocpIfCSHardwareIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, ocpIfCSGetNumberControllers );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, ocpIfCSGetControllersNames );

	#else
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_READ, ocpIfDualCoreTraceRead );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_RESET, ocpIfDualCoreTraceReset );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIZE, ocpIfDualCoreTraceGetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_SET_SIZE, ocpIfDualCoreTraceSetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, ocpIfDualCoreTraceGetNumberSignals );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, ocpIfDualCoreTraceGetSignalsNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, ocpIfDualCoreTraceGetNumberTraces );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_TRACES_NAMES, ocpIfDualCoreTraceGetTracesNames );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_STATUS, ocpIfDualCoreCSStatus );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_ENABLE, ocpIfDualCoreCSEnable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_DISABLE, ocpIfDualCoreCSDisable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_CONTROLLER_IF, ocpIfDualCoreCSControllerIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_HARDWARE_IF, ocpIfDualCoreCSHardwareIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, ocpIfDualCoreCSGetNumberControllers );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, ocpIfDualCoreCSGetControllersNames );
#endif

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_PLATFORM_ID, ocpIfPlatformID );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_PLATFORM_IF, ocpIfPlatformIf );

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
#if( OCP_IF_CONFIG_CORE == 0 )
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	size_t address;
	uint32_t size;
	int32_t status;

	id = *( (uint32_t *)in );

	status = ocpTraceGetAddress( id, (void *)(&address) );
	if( status < 0 ) return status;

	size = ocpTraceGetSize( id );
	if( size < 0 ) return size;

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
	int32_t size;
	uint32_t *o = (uint32_t *)*out;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	size = ocpTraceGetSize(id);
	if( size < 0 ) return size;

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
	if( n < 0 ) return n;

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

	size = ocpTraceGetSignalsNames(id, o, maxoutsize);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t n;

	uint32_t *o = (uint32_t *)( *out );

	n = ocpTraceGetNumberTraces();
	if( n < 0 ) return n;

	*o = n;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpTraceGetTracesNames(o, maxoutsize);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetAddress(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	size_t address;
	int32_t status;

	size_t *o = (size_t *)( *out );

	id = *( (uint32_t *)in );

	status = ocpTraceGetAddress( id, (void *)(&address) );
	if( status < 0 ) return status;

	*o = address;

	return sizeof(size_t);
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;
	int32_t cmdStatus;

	uint32_t *o = (uint32_t *)( *out );

	id = *( (uint32_t *)in );

	cmdStatus = ocpCSStatus(id, &status);
	if( cmdStatus < 0 ) return cmdStatus;

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

	return ocpCSControllerInterface(id, (void *)p, insize - 4, out, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	return ocpCSHardwareInterface(id, (void *)p, insize - 4, out, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t n;

	uint32_t *o = (uint32_t *)( *out );

	n = ocpCSGetNumberControllers();
	if( n < 0 ) return n;

	*o = n;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpCSGetControllersNames(o, maxoutsize);

	return size;
}

//-----------------------------------------------------------------------------
#else
static int32_t ocpIfDualCoreTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	size_t address;
	uint32_t size;
	int32_t status;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfTraceGetAddressSecondCore( id, (void *)(&address) );
		if( status < 0 ) return status;

		size = ocpIfTraceGetSizeSecondCore( id );
		if( size < 0 ) return size;
	}
	else{
		status = ocpTraceGetAddress( id, (void *)(&address) );
		if( status < 0 ) return status;

		size = ocpTraceGetSize( id );
		if( size < 0 ) return size;
	}

	*out = (void *)( address );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfTraceResetSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		status = ocpTraceReset(id);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t *o = (int32_t *)*out;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		size = ocpIfTraceGetSizeSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		size = ocpTraceGetSize(id);
	}

	if( size < 0 ) return size;
	*o = size;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t status;
	int32_t *p = (int32_t *)in;
	int32_t nTracesSecondCore;

	id = *p++;
	size = *p;

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfTraceSetSizeSecondCore(id, size);
	}
	else{
		id = id - nTracesSecondCore;
		status = ocpTraceSetSize(id, size);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t nSignals;
	int32_t *o = (int32_t *)*out;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		nSignals = ocpIfTraceGetNumberSignalsSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		nSignals = ocpTraceGetNumberSignals(id);
	}

	if( nSignals < 0 ) return nSignals;
	*o = nSignals;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t nTracesSecondCore;

	char *o = (char *)( *out );

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		size = ocpIfTraceGetSignalsNamesSecondCore(id, o, maxoutsize);
	}
	else{
		id = id - nTracesSecondCore;
		size = ocpTraceGetSignalsNames(id, o, maxoutsize);
	}

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t *o = (uint32_t *)( *out );

	int32_t nTracesSecondCore;
	int32_t nTracesThisCore;

	nTracesSecondCore = ocpIfTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	nTracesThisCore = ocpTraceGetNumberTraces();
	if( nTracesThisCore < 0 ) return nTracesSecondCore;

	*o = nTracesThisCore + nTracesSecondCore;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t sizeSecondCore;
	int32_t sizeThisCore;

	char *o = (char *)( *out );

	sizeSecondCore = ocpIfTraceGetTracesNamesSecondCore(o, maxoutsize);
	if( sizeSecondCore < 0 ) return sizeSecondCore;

	o = o + sizeSecondCore;
	sizeThisCore = ocpTraceGetTracesNames(o, maxoutsize - sizeSecondCore);
	if( sizeThisCore < 0 ) return sizeThisCore;

	return sizeThisCore + sizeSecondCore;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;
	int32_t cmdStatus;

	int32_t nControllersSecondCore;

	uint32_t *o = (uint32_t *)( *out );

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		cmdStatus = ocpIfCSStatusSecondCore(id, &status);
	}
	else{
		id = id - nControllersSecondCore;
		cmdStatus = ocpCSStatus(id, &status);
	}

	if( cmdStatus < 0 ) return cmdStatus;

	*o = status;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nControllersSecondCore;

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfCSEnableSecondCore(id);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSEnable(id);
	}

	if( status < 0 ) return status;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nControllersSecondCore;

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfCSDisableSecondCore(id);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSDisable(id);
	}

	if( status < 0 ) return status;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t status;
	int32_t nControllersSecondCore;

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfCSControllerInterfaceSecondCore(id, (void *)p, insize - 4, out, maxoutsize);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSControllerInterface(id, (void *)p, insize - 4, out, maxoutsize);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t status;
	int32_t nControllersSecondCore;

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfCSHardwareInterfaceSecondCore(id, (void *)p, insize - 4, out, maxoutsize);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSHardwareInterface(id, (void *)p, insize - 4, out, maxoutsize);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t nControllersSecondCore, nControllersThisCore;

	uint32_t *o = (uint32_t *)( *out );

	nControllersSecondCore = ocpIfCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	nControllersThisCore = ocpCSGetNumberControllers();
	if( nControllersThisCore < 0 ) return nControllersThisCore;

	*o = nControllersThisCore + nControllersSecondCore;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfDualCoreCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t sizeSecondCore;
	int32_t sizeThisCore;

	char *o = (char *)( *out );

	sizeSecondCore = ocpIfCSGetControllersNamesSecondCore(o, maxoutsize);
	if( sizeSecondCore < 0 ) return sizeSecondCore;

	o = o + sizeSecondCore;
	sizeThisCore = ocpCSGetControllersNames(o, maxoutsize - sizeSecondCore);
	if( sizeThisCore < 0 ) return sizeThisCore;

	return sizeThisCore + sizeSecondCore;

}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberTracesSecondCore(void){

	int32_t status;

	uint32_t nTracesSecondCore;
	uint32_t *p;
	uint32_t cmd;

	cmd = OCP_IF_CMD_TRACE_GET_NUMBER_TRACES;
	p = &nTracesSecondCore;
	status = ipcClientRequest( (void *)&cmd, 4, (void **)&p, 4, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );
	if( status < 0 ) return status;

	return nTracesSecondCore;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceResetSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd;

	cmd = OCP_IF_CMD_TRACE_RESET;

	status = ipcClientRequest( (void *)&cmd, 4, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSizeSecondCore(uint32_t id){

	int32_t size;
	int32_t status;
	int32_t *p;
	uint32_t cmd[2];

	p = &size;

	cmd[0] = OCP_IF_CMD_TRACE_GET_SIZE;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, (void **)&p, 4, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );
	if( status < 0 ) return status;

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceSetSizeSecondCore(uint32_t id, int32_t size){

	int32_t status;
	uint32_t cmd[3];

	cmd[0] = OCP_IF_CMD_TRACE_SET_SIZE;
	cmd[1] = id;
	cmd[2] = size;
	status = ipcClientRequest( (void *)&cmd, 12, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetNumberSignalsSecondCore(uint32_t id){

	int32_t status;
	int32_t nSignals;
	uint32_t cmd[2];
	int32_t *p;

	cmd[0] = OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS;
	cmd[1] = id;

	p = &nSignals;
	status = ipcClientRequest( (void *)&cmd, 8, (void **)&p, 4, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );
	if( status < 0 ) return status;

	return nSignals;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetSignalsNamesSecondCore(uint32_t id,
		char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES;
	cmd[1] = id;

	size = ipcClientRequest( (void *)&cmd, 8, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetTracesNamesSecondCore(char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd;

	cmd = OCP_IF_CMD_TRACE_GET_TRACES_NAMES;

	size = ipcClientRequest( (void *)&cmd, 4, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfTraceGetAddressSecondCore(uint32_t id, void *address){

	int32_t size;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_TRACE_GET_ADDRESS;
	cmd[1] = id;

	size = ipcClientRequest( (void *)&cmd, 8, (void **)&address, sizeof(void *), OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetNumberControllersSecondCore(void){

	int32_t status;

	uint32_t nControllersSecondCore;
	uint32_t *p;
	uint32_t cmd;

	cmd = OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS;
	p = &nControllersSecondCore;
	status = ipcClientRequest( (void *)&cmd, 4, (void **)&p, 4, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );
	if( status < 0 ) return status;

	return nControllersSecondCore;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSStatusSecondCore(uint32_t id, int32_t *status){

	int32_t cmdStatus;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_STATUS;
	cmd[1] = id;

	cmdStatus = ipcClientRequest( (void *)&cmd, 8, (void **)&status, sizeof(void *), OCP_IF_CONFIG_DUAL_CORE_COMM_TO );
	if( cmdStatus < 0 ) return cmdStatus;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSEnableSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_ENABLE;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSDisableSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_DISABLE;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSGetControllersNamesSecondCore(char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd;

	cmd = OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES;

	size = ipcClientRequest( (void *)&cmd, 4, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSControllerInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t size;
	uint32_t k;
	char buffer[insize + 8];
	char *pin;
	char *pb = buffer;

	*( (uint32_t *)&buffer[0] ) = OCP_IF_CMD_CS_CONTROLLER_IF;
	*( (uint32_t *)&buffer[4] ) = id;

	pin = (char *)in;
	pb = &buffer[8];
	while( k++ < insize ) *pb++ = *pin++;

	size = ipcClientRequest( (void *)buffer, insize + 8, out, maxoutsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfCSHardwareInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t size;
	uint32_t k;
	char buffer[insize + 8];
	char *pin;
	char *pb = buffer;

	*( (uint32_t *)&buffer[0] ) = OCP_IF_CMD_CS_HARDWARE_IF;
	*( (uint32_t *)&buffer[4] ) = id;

	pin = (char *)in;
	pb = &buffer[8];
	while( k++ < insize ) *pb++ = *pin++;

	size = ipcClientRequest( (void *)buffer, insize + 8, out, maxoutsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
static int32_t ocpIfPlatformID(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpPlatformID(o, maxoutsize);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfPlatformIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	return ocpPlatformIf(in, insize, out, maxoutsize);
}
//=============================================================================
