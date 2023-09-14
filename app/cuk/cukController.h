/*
 * cukController.h
 *
 *  Created on: 11.09.2023
 *      Author: LRS
 */

#ifndef cuk_CONTROLLER_H_
#define cuk_CONTROLLER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
	CUK_CONTROLLER_DISABLED,
    CUK_CONTROLLER_OL,
	CUK_CONTROLLER_END
}cukControllers_t;

typedef enum{
	CUK_CONTROLLER_IF_SET,			/* Sets the active controller */
	CUK_CONTROLLER_IF_GET,			/* Gets the active controller */
	CUK_CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	CUK_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	CUK_CONTROLLER_IF_RESET,        /* Resets the specified controller */
	CUK_CONTROLLER_IF_END
}cukControllerInterface_t;

#define CUK_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define CUK_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define CUK_CONTROLLER_ERR_INACTIVE_CTL	    -3	/* No controller active when trying to execute run function */
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControllerInitialize(void);
//-----------------------------------------------------------------------------
int32_t cukControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t cukControllerRun(void *inputs, int32_t ninputs, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t cukControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_CONTROLLER_H_ */
