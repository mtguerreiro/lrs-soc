/*
 * controller.h
 *
 *  Created on: 10 de set de 2022
 *      Author: marco
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
	CONTROLLER_PID,
	CONTROLLER_END
}controllerTypesEnum_t;

typedef enum{
	CONTROLLER_IF_SET,			/* Sets the active controller */
	CONTROLLER_IF_GET,			/* Gets the active controller */
	CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	CONTROLLER_IF_END
}controllerInterface_t;

#define CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define CONTROLLER_ERR_INACTIVE_CTL		-3	/* No controller active when trying to execute run function */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controllerInitialize(void);
//-----------------------------------------------------------------------------
int32_t controllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t controllerRun(void *inputs, uint32_t ninputs, void *meas, uint32_t nmeas, void *outputs);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CONTROLLER_H_ */
