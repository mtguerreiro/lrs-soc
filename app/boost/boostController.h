/*
 * boostController.h
 *
 */

#ifndef BOOST_CONTROLLER_H_
#define BOOST_CONTROLLER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
	BOOST_CONTROLLER_DISABLED,
    BOOST_CONTROLLER_STARTUP,
	BOOST_CONTROLLER_END
}boostControllers_t;

typedef enum{
	BOOST_CONTROLLER_IF_SET,			/* Sets the active controller */
	BOOST_CONTROLLER_IF_GET,			/* Gets the active controller */
	BOOST_CONTROLLER_IF_SET_PARAMS,	/* Sets parameters for the specified controller */
	BOOST_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	BOOST_CONTROLLER_IF_RESET,        /* Resets the specified controller */
	BOOST_CONTROLLER_IF_SET_REF,      /* Sets the references */
	BOOST_CONTROLLER_IF_GET_REF,         /* Gets the references */
	BOOST_CONTROLLER_IF_END
}boostControllerInterface_t;

#define BOOST_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define BOOST_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define BOOST_CONTROLLER_ERR_INACTIVE_CTL	    -3	/* No controller active when trying to execute run function */

typedef void (*boostControllerEnable_t)(void);
typedef void (*boostControllerDisable_t)(void);

typedef struct{
    boostControllerEnable_t enable;
    boostControllerDisable_t disable;
}boostControllerConfig_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void boostControllerInitialize(boostControllerConfig_t *config);
//-----------------------------------------------------------------------------
int32_t boostControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t boostControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t boostControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* BOOST_CONTROLLER_H_ */
