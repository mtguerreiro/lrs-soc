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
#include "ocpIfMaster.h"

#include "ocpTraceMaster.h"
#include "ocpCSMaster.h"
#include "ocpPlatform.h"
#include "ocpConfig.h"
#include "ocpOpil.h"

#include "stddef.h"

#include "../utils/rp.h"

#include "ocp/ipc/ipcClient.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberTracesSecondCore(void);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceResetSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSizeSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceSetSizeSecondCore(uint32_t id, int32_t size);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberSignalsSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSignalsNamesSecondCore(uint32_t id,
		char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetTracesNamesSecondCore(char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetAddressSecondCore(uint32_t id, void *address);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetNumberControllersSecondCore(void);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSStatusSecondCore(uint32_t id, int32_t *status);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSEnableSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSDisableSecondCore(uint32_t id);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetControllersNamesSecondCore(char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSControllerInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSHardwareInterfaceSecondCore(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterPlatformID(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterPlatformIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Definitions ------------------------------*/
//=============================================================================

typedef struct{

	rphandle_t handles[OCP_IF_CMD_END];
	rpctx_t rp;

}ocpIfControl_t;

#define OCP_IF_CONFIG_DUAL_CORE_COMM_TO		60000
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
int32_t ocpIfMasterInitialize(void){

	rpInitialize( &xcontrol.rp, OCP_IF_CMD_END, xcontrol.handles );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_READ, ocpIfMasterTraceRead );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_RESET, ocpIfMasterTraceReset );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIZE, ocpIfMasterTraceGetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_SET_SIZE, ocpIfMasterTraceSetSize );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_SIGNALS, ocpIfMasterTraceGetNumberSignals );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES, ocpIfMasterTraceGetSignalsNames );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_NUMBER_TRACES, ocpIfMasterTraceGetNumberTraces );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_TRACE_GET_TRACES_NAMES, ocpIfMasterTraceGetTracesNames );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_STATUS, ocpIfMasterCSStatus );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_ENABLE, ocpIfMasterCSEnable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_DISABLE, ocpIfMasterCSDisable );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_CONTROLLER_IF, ocpIfMasterCSControllerIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_HARDWARE_IF, ocpIfMasterCSHardwareIf );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_NUMBER_CONTROLLERS, ocpIfMasterCSGetNumberControllers );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES, ocpIfMasterCSGetControllersNames );

	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_PLATFORM_ID, ocpIfMasterPlatformID );
	rpRegisterHandle( &xcontrol.rp, OCP_IF_CMD_PLATFORM_IF, ocpIfMasterPlatformIf );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpIfMaster(void *in, int32_t insize, void **out, int32_t maxoutsize){

	return rpRequest( &xcontrol.rp, in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceRead(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	size_t address;
	uint32_t size;
	int32_t status;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfMasterTraceGetAddressSecondCore( id, (void *)(&address) );
		if( status < 0 ) return status;

		size = ocpIfMasterTraceGetSizeSecondCore( id );
		if( size < 0 ) return size;
	}
	else{
		status = ocpTraceMasterGetAddress( id, (void *)(&address) );
		if( status < 0 ) return status;

		size = ocpTraceMasterGetSize( id );
		if( size < 0 ) return size;
	}

	*out = (void *)( address );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceReset(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfMasterTraceResetSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		status = ocpTraceMasterReset(id);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t *o = (int32_t *)*out;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		size = ocpIfMasterTraceGetSizeSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		size = ocpTraceMasterGetSize(id);
	}

	if( size < 0 ) return size;
	*o = size;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceSetSize(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t status;
	int32_t *p = (int32_t *)in;
	int32_t nTracesSecondCore;

	id = *p++;
	size = *p;

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		status = ocpIfMasterTraceSetSizeSecondCore(id, size);
	}
	else{
		id = id - nTracesSecondCore;
		status = ocpTraceMasterSetSize(id, size);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberSignals(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t nSignals;
	int32_t *o = (int32_t *)*out;
	int32_t nTracesSecondCore;

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		nSignals = ocpIfMasterTraceGetNumberSignalsSecondCore(id);
	}
	else{
		id = id - nTracesSecondCore;
		nSignals = ocpTraceMasterGetNumberSignals(id);
	}

	if( nSignals < 0 ) return nSignals;
	*o = nSignals;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSignalsNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t size;
	int32_t nTracesSecondCore;

	char *o = (char *)( *out );

	id = *( (uint32_t *)in );

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	if( id < nTracesSecondCore ){
		size = ocpIfMasterTraceGetSignalsNamesSecondCore(id, o, maxoutsize);
	}
	else{
		id = id - nTracesSecondCore;
		size = ocpTraceMasterGetSignalsNames(id, o, maxoutsize);
	}

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberTraces(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t *o = (uint32_t *)( *out );

	int32_t nTracesSecondCore;
	int32_t nTracesThisCore;

	nTracesSecondCore = ocpIfMasterTraceGetNumberTracesSecondCore();
	if( nTracesSecondCore < 0 ) return nTracesSecondCore;

	nTracesThisCore = ocpTraceMasterGetNumberTraces();
	if( nTracesThisCore < 0 ) return nTracesSecondCore;

	*o = nTracesThisCore + nTracesSecondCore;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetTracesNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t sizeSecondCore;
	int32_t sizeThisCore;

	char *o = (char *)( *out );

	sizeSecondCore = ocpIfMasterTraceGetTracesNamesSecondCore(o, maxoutsize);
	if( sizeSecondCore < 0 ) return sizeSecondCore;

	o = o + sizeSecondCore;
	sizeThisCore = ocpTraceMasterGetTracesNames(o, maxoutsize - sizeSecondCore);
	if( sizeThisCore < 0 ) return sizeThisCore;

	return sizeThisCore + sizeSecondCore;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSStatus(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;

	int32_t status;
	int32_t cmdStatus;

	int32_t nControllersSecondCore;

	uint32_t *o = (uint32_t *)( *out );

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		cmdStatus = ocpIfMasterCSStatusSecondCore(id, &status);
	}
	else{
		id = id - nControllersSecondCore;
		cmdStatus = ocpCSMasterStatus(id, &status);
	}

	if( cmdStatus < 0 ) return cmdStatus;

    *o = status;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSEnable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nControllersSecondCore;

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfMasterCSEnableSecondCore(id);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSMasterEnable(id);
	}

	if( status < 0 ) return status;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSDisable(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t id;
	int32_t status;
	int32_t nControllersSecondCore;

	id = *( (uint32_t *)in );

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfMasterCSDisableSecondCore(id);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSMasterDisable(id);
	}

	if( status < 0 ) return status;

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSControllerIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t status;
	int32_t nControllersSecondCore;

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfMasterCSControllerInterfaceSecondCore(id, (void *)p, insize - 4, out, maxoutsize);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSMasterControllerInterface(id, (void *)p, insize - 4, out, maxoutsize);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSHardwareIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t status;
	int32_t nControllersSecondCore;

	uint32_t id;
	uint32_t *p = (uint32_t *)in;

	id = *p++;

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	if( id < nControllersSecondCore ) {
		status = ocpIfMasterCSHardwareInterfaceSecondCore(id, (void *)p, insize - 4, out, maxoutsize);
	}
	else{
		id = id - nControllersSecondCore;
		status = ocpCSMasterHardwareInterface(id, (void *)p, insize - 4, out, maxoutsize);
	}

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetNumberControllers(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t nControllersSecondCore, nControllersThisCore;

	uint32_t *o = (uint32_t *)( *out );

	nControllersSecondCore = ocpIfMasterCSGetNumberControllersSecondCore();
	if( nControllersSecondCore < 0 ) return nControllersSecondCore;

	nControllersThisCore = ocpCSMasterGetNumberControllers();
	if( nControllersThisCore < 0 ) return nControllersThisCore;

	*o = nControllersThisCore + nControllersSecondCore;

	return 4;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetControllersNames(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	int32_t sizeSecondCore;
	int32_t sizeThisCore;

	char *o = (char *)( *out );

	sizeSecondCore = ocpIfMasterCSGetControllersNamesSecondCore(o, maxoutsize);
	if( sizeSecondCore < 0 ) return sizeSecondCore;

	o = o + sizeSecondCore;
	sizeThisCore = ocpCSMasterGetControllersNames(o, maxoutsize - sizeSecondCore);
	if( sizeThisCore < 0 ) return sizeThisCore;

	return sizeThisCore + sizeSecondCore;

}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberTracesSecondCore(void){

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
static int32_t ocpIfMasterTraceResetSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_TRACE_RESET;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetSizeSecondCore(uint32_t id){

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
static int32_t ocpIfMasterTraceSetSizeSecondCore(uint32_t id, int32_t size){

	int32_t status;
	uint32_t cmd[3];

	cmd[0] = OCP_IF_CMD_TRACE_SET_SIZE;
	cmd[1] = id;
	cmd[2] = size;
	status = ipcClientRequest( (void *)&cmd, 12, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetNumberSignalsSecondCore(uint32_t id){

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
static int32_t ocpIfMasterTraceGetSignalsNamesSecondCore(uint32_t id,
		char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_TRACE_GET_SIGNALS_NAMES;
	cmd[1] = id;

	size = ipcClientRequest( (void *)&cmd, 8, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetTracesNamesSecondCore(char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd;

	cmd = OCP_IF_CMD_TRACE_GET_TRACES_NAMES;

	size = ipcClientRequest( (void *)&cmd, 4, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterTraceGetAddressSecondCore(uint32_t id, void *address){

	int32_t size;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_TRACE_GET_ADDRESS;
	cmd[1] = id;

	size = ipcClientRequest( (void *)&cmd, 8, (void **)&address, sizeof(void *), OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetNumberControllersSecondCore(void){

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
static int32_t ocpIfMasterCSStatusSecondCore(uint32_t id, int32_t *status){

	int32_t cmdStatus;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_STATUS;
	cmd[1] = id;

	cmdStatus = ipcClientRequest( (void *)&cmd, 8, (void **)&status, sizeof(void *), OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return cmdStatus;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSEnableSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_ENABLE;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSDisableSecondCore(uint32_t id){

	int32_t status;
	uint32_t cmd[2];

	cmd[0] = OCP_IF_CMD_CS_DISABLE;
	cmd[1] = id;

	status = ipcClientRequest( (void *)&cmd, 8, 0, 0, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSGetControllersNamesSecondCore(char *buffer, int32_t maxsize){

	int32_t size;
	uint32_t cmd;

	cmd = OCP_IF_CMD_CS_GET_CONTROLLERS_NAMES;

	size = ipcClientRequest( (void *)&cmd, 4, (void **)&buffer, maxsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSControllerInterfaceSecondCore(uint32_t id,
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
	k = 0;
	while( k++ < insize ) *pb++ = *pin++;

	size = ipcClientRequest( (void *)buffer, insize + 8, out, maxoutsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterCSHardwareInterfaceSecondCore(uint32_t id,
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
	k = 0;
	while( k++ < insize ) *pb++ = *pin++;

	size = ipcClientRequest( (void *)buffer, insize + 8, out, maxoutsize, OCP_IF_CONFIG_DUAL_CORE_COMM_TO );

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterPlatformID(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	uint32_t size;

	char *o = (char *)( *out );

	size = ocpPlatformID(o, maxoutsize);

	return size;
}
//-----------------------------------------------------------------------------
static int32_t ocpIfMasterPlatformIf(void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	return ocpPlatformIf(in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
//=============================================================================
