/*
 * buckController.h
 *
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
    BUCK_CONTROLLER_STARTUP,
    BUCK_CONTROLLER_SFB_INT,
	BUCK_CONTROLLER_END
}buckControllers_t;

typedef enum{
	BUCK_CONTROLLER_IF_SET,			/* Sets the active controller */
	BUCK_CONTROLLER_IF_GET,			/* Gets the active controller */
	BUCK_CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	BUCK_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	BUCK_CONTROLLER_IF_RESET,        /* Resets the specified controller */
	BUCK_CONTROLLER_IF_SET_REF,      /* Sets the references */
	BUCK_CONTROLLER_IF_GET_REF,         /* Gets the references */
	BUCK_CONTROLLER_IF_END
}buckControllerInterface_t;

#define BUCK_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define BUCK_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define BUCK_CONTROLLER_ERR_INACTIVE_CTL	    -3	/* No controller active when trying to execute run function */

typedef void (*buckControllerEnable_t)(void);
typedef void (*buckControllerDisable_t)(void);

typedef struct{
    buckControllerEnable_t enable;
    buckControllerDisable_t disable;
}buckControllerConfig_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void buckControllerInitialize(buckControllerConfig_t *config);
//-----------------------------------------------------------------------------
int32_t buckControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t buckControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t buckControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BUCK_CONTROLLER_H_ */
