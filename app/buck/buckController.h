/*
 * buckController.h
 *
 *  Created on: 30 de ago de 2023
 *      Author: marco
 */

#ifndef BUCK_CONTROLLER_H_
#define BUCK_CONTROLLER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
	BUCK_CONTROLLER_DISABLED,
    BUCK_CONTROLLER_SFB,
	BUCK_CONTROLLER_END
}buckControllers_t;

typedef enum{
	BUCK_CONTROLLER_IF_SET,			/* Sets the active controller */
	BUCK_CONTROLLER_IF_GET,			/* Gets the active controller */
	BUCK_CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	BUCK_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	BUCK_CONTROLLER_IF_RESET,       /* Resets the specified controller */
	BUCK_CONTROLLER_IF_END
}buckControllerInterface_t;

#define BUCK_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define BUCK_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define BUCK_CONTROLLER_ERR_INACTIVE_CTL	-3	/* No controller active when trying to execute run function */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControllerInitialize(void);
//-----------------------------------------------------------------------------
int32_t buckControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t buckControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t buckControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BUCK_CONTROLLER_H_ */
