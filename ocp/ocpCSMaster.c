/*
 * ocpCSMaster.c
 *
 *  Created on: 23.03.2023
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpCSMaster.h"

#include "ocpConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef controlsys_t ocpCSMaster_t;

typedef struct ocpCSMasterControl_t{

	ocpCSMaster_t cs[OCP_CS_MASTER_END];

	char names[OCP_CS_MASTER_END * OCP_CS_MASTER_CONFIG_CS_NAME_MAX_LEN];

	char *np;

}ocpCSMasterControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpCSMasterControl_t xcscontrol = {.np = xcscontrol.names};

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpCSMasterInitialize(uint32_t id, ocpCSMasterConfig_t *config, char *name){

	char *tplim;

	if( id >= OCP_CS_MASTER_END ) return -1;

	controlsysInitialize( &xcscontrol.cs[id], config );

	tplim = xcscontrol.names + OCP_CS_MASTER_END * OCP_CS_MASTER_CONFIG_CS_NAME_MAX_LEN;
	while( *name && (xcscontrol.np < tplim) ) *xcscontrol.np++ = *name++;
	*xcscontrol.np++ = 0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterControllerInterface(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( id >= OCP_CS_MASTER_END ) return -1;

	return controlsysControllerInterface( &xcscontrol.cs[id], in, insize, out, maxoutsize );
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterHardwareInterface(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( id >= OCP_CS_MASTER_END ) return -1;

	return controlsysHardwareInterface( &xcscontrol.cs[id], in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterRun(uint32_t id){

	if( id >= OCP_CS_MASTER_END ) return -1;

	return controlsysRun( &xcscontrol.cs[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterEnable(uint32_t id){

	if( id >= OCP_CS_MASTER_END ) return -1;

	controlsysEnable( &xcscontrol.cs[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterDisable(uint32_t id){

	if( id >= OCP_CS_MASTER_END ) return -1;

	controlsysDisable( &xcscontrol.cs[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterStatus(uint32_t id, int32_t *status){

	if( id >= OCP_CS_MASTER_END ) return -1;

	*status = controlsysStatus(&xcscontrol.cs[id]);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterGetNumberControllers(void){

	return OCP_CS_MASTER_END;
}
//-----------------------------------------------------------------------------
int32_t ocpCSMasterGetControllersNames(char *buffer, int32_t maxsize){

	int32_t k;
	char *p;

	p = xcscontrol.names;
	k = 0;
	while( (p < xcscontrol.np) && (k < maxsize) ){
		*buffer++ = *p++;
		k++;
	}

	return k;
}
//-----------------------------------------------------------------------------
//=============================================================================
