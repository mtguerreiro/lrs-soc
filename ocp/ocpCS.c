/*
 * ocpCS.c
 *
 *  Created on: 17.02.2023
 *      Author: mguerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpCS.h"

//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef controlsys_t ocpCS_t;

typedef struct ocpCSControl_t{

	ocpCS_t cs[OCP_CS_END];

	char names[OCP_CS_END * OCP_CS_CONFIG_CS_NAME_MAX_LEN];

	char *np;

}ocpCSControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================

static ocpCSControl_t xcscontrol = {.np = xcscontrol.names};

//=============================================================================


//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpCSInitialize(uint32_t id, ocpCSConfig_t *config, char *name){

	char *tplim;

	if( id >= OCP_CS_END ) return -1;

	controlsysInitialize( &xcscontrol.cs[id], config );

	tplim = xcscontrol.names + OCP_CS_END * OCP_CS_CONFIG_CS_NAME_MAX_LEN;
	while( *name && (xcscontrol.np < tplim) ) *xcscontrol.np++ = *name++;
	*xcscontrol.np++ = 0;

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSControllerInterface(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( id >= OCP_CS_END ) return -1;

	return controlsysControllerInterface( &xcscontrol.cs[id], in, insize, out, maxoutsize );
}
//-----------------------------------------------------------------------------
int32_t ocpCSHardwareInterface(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize){

	if( id >= OCP_CS_END ) return -1;

	return controlsysHardwareInterface( &xcscontrol.cs[id], in, insize, out, maxoutsize);
}
//-----------------------------------------------------------------------------
int32_t ocpCSRun(uint32_t id){

	if( id >= OCP_CS_END ) return -1;

	return controlsysRun( &xcscontrol.cs[id] );
}
//-----------------------------------------------------------------------------
int32_t ocpCSEnable(uint32_t id){

	if( id >= OCP_CS_END ) return -1;

	controlsysEnable( &xcscontrol.cs[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSDisable(uint32_t id){

	if( id >= OCP_CS_END ) return -1;

	controlsysDisable( &xcscontrol.cs[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSStatus(uint32_t id, int32_t *status){

	if( id >= OCP_CS_END ) return -1;

	*status = controlsysStatus( &xcscontrol.cs[id] );

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpCSGetNumberControllers(void){

	return OCP_CS_END;
}
//-----------------------------------------------------------------------------
int32_t ocpCSGetControllersNames(char *buffer, int32_t maxsize){

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
