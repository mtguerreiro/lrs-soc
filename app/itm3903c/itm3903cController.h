/*
 * itm3903cController.h
 *
 */

#ifndef ITM3903C_CONTROLLER_H_
#define ITM3903C_CONTROLLER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum{
	ITM3903C_CONTROLLER_DISABLED,
	ITM3903C_CONTROLLER_OL,
	ITM3903C_CONTROLLER_END
}itm3903cControllers_t;

typedef enum{
	ITM3903C_CONTROLLER_IF_SET,			/* Sets the active controller */
	ITM3903C_CONTROLLER_IF_GET,			/* Gets the active controller */
	ITM3903C_CONTROLLER_IF_SET_PARAMS,  /* Sets parameters for the specified controller */
	ITM3903C_CONTROLLER_IF_GET_PARAMS,	/* Gets parameters for the specified controller */
	ITM3903C_CONTROLLER_IF_RESET,       /* Resets the specified controller */
	ITM3903C_CONTROLLER_IF_SET_REF,     /* Sets the references */
	ITM3903C_CONTROLLER_IF_GET_REF,     /* Gets the references */
	ITM3903C_CONTROLLER_IF_END
}itm3903cControllerInterface_t;

#define ITM3903C_CONTROLLER_ERR_INVALID_CMD		-1	/* Invalid command */
#define ITM3903C_CONTROLLER_ERR_INVALID_CTL		-2	/* Invalid controller */
#define ITM3903C_CONTROLLER_ERR_INACTIVE_CTL	-3	/* No controller active when trying to execute run function */


typedef void (*itm3903cControllerEnable_t)(void);
typedef void (*itm3903cControllerDisable_t)(void);

typedef struct{
    itm3903cControllerEnable_t enable;
    itm3903cControllerDisable_t disable;
}itm3903cControllerConfig_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void itm3903cControllerInitialize(itm3903cControllerConfig_t *config);
//-----------------------------------------------------------------------------
int32_t itm3903cControllerInterface(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t itm3903cControllerRun(void *meas, int32_t nmeas, void *outputs, int32_t nmaxoutputs);
//-----------------------------------------------------------------------------
int32_t itm3903cControllerStatus(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ITM3903C_CONTROLLER_H_ */
