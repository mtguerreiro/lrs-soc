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
    CUK_CONTROLLER_STARTUP,
	CUK_CONTROLLER_OL,
    CUK_CONTROLLER_ENERGY,
    CUK_CONTROLLER_ENERGY_INT,
    CUK_CONTROLLER_SFB,
    CUK_CONTROLLER_SFB_INT,
    CUK_CONTROLLER_PCH,
	CUK_CONTROLLER_END
}cukControllers_t;

typedef enum{
	CUK_CONTROLLER_IF_SET,			/* Sets the active controller */
	CUK_CONTROLLER_IF_GET,			/* Gets the active controller */
	CUK_CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	CUK_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	CUK_CONTROLLER_IF_RESET,        /* Resets the specified controller */
	CUK_CONTROLLER_IF_SET_REF,      /* Sets the references */
	CUK_CONTROLLER_IF_GET_REF,         /* Gets the references */
	CUK_CONTROLLER_IF_END
}cukControllerInterface_t;

#define CUK_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define CUK_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define CUK_CONTROLLER_ERR_INACTIVE_CTL	    -3	/* No controller active when trying to execute run function */


typedef void (*cukControllerEnable_t)(void);
typedef void (*cukControllerDisable_t)(void);

typedef struct{
    cukControllerEnable_t enable;
    cukControllerDisable_t disable;
}cukControllerConfig_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void cukControllerInitialize(cukControllerConfig_t *config);
//-----------------------------------------------------------------------------
int32_t cukControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t cukControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t cukControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CUK_CONTROLLER_H_ */
