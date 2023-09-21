/*
 * controlsys.h
 *
 *  Created on: 04.09.2022
 *      Author: mguerreiro
 *
 */

#ifndef CONTROLSYS_H_
#define CONTROLSYS_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "control.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
typedef enum controlsysStatus_t{
    CONTROLSYS_STATUS_ENABLED,
	CONTROLSYS_STATUS_DISABLED,
	CONTROLSYS_STATUS_HARDWARE_ERROR,
	CONTROLSYS_STATUS_CONTROLLER_ERROR,
	CONTROLSYS_STATUS_UNKNOWN_RUN_ERROR
}controlsysStatus_t;

typedef controlOnEntry_t controlsysOnEntry_t;
typedef controlGetInputs_t controlsysGetInputs_t;
typedef controlRun_t controlsysRun_t;
typedef controlApplyOutputs_t controlsysApplyOutputs_t;
typedef controlOnExit_t controlsysOnExit_t;

/* Controller interface */
typedef int32_t(*controlsysControllerInterface_t)(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

/* Controller status */
typedef int32_t(*controlsysControllerStatus_t)(void);

/* Hardware interface */
typedef int32_t(*controlsysHardwareInterface_t)(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

/* Hardware status */
typedef int32_t(*controlsysHardwareStatus_t)(void);

/* Enable sampling */
typedef void(*controlsysEnable_t)(void);

/* Disable sampling */
typedef void(*controlsysDisable_t)(void);

typedef struct {

	void *binputs;
	void *bprocInputs;
	void *boutputs;
	void *bprocOutputs;

	controlsysGetInputs_t fgetInputs;
	controlsysApplyOutputs_t fapplyOutputs;
	controlsysHardwareInterface_t fhwInterface;
	controlsysHardwareStatus_t fhwStatus;

	controlsysRun_t frun;
	controlsysControllerInterface_t fcontrollerInterface;
	controlsysControllerStatus_t fcontrollerStatus;

	controlsysOnEntry_t fonEntry;
	controlsysOnEntry_t fonExit;

	controlsysEnable_t fenable;
	controlsysDisable_t fdisable;

}controlsysConfig_t;

typedef struct {

	control_t control;

	controlsysHardwareInterface_t fhwInterface;
	controlsysHardwareStatus_t fhwStatus;

	controlsysControllerInterface_t fcontrollerInterface;
	controlsysControllerStatus_t fcontrollerStatus;

	controlsysEnable_t fenable;
	controlsysDisable_t fdisable;

	controlsysStatus_t status;
}controlsys_t;

#define CONTROLSYS_ERR_UNDEF		-1000
#define CONTROLSYS_ERR_DISABLED		-1001
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void controlsysInitialize(controlsys_t *sys, controlsysConfig_t *config);
//-----------------------------------------------------------------------------
int32_t controlsysControllerInterface(controlsys_t *sys,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t controlsysHardwareInterface(controlsys_t *sys,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t controlsysRun(controlsys_t *sys);
//-----------------------------------------------------------------------------
int32_t controlsysEnable(controlsys_t *sys);
//-----------------------------------------------------------------------------
void controlsysDisable(controlsys_t *sys);
//-----------------------------------------------------------------------------
int32_t controlsysStatus(controlsys_t *sys);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* CONTROLSYS_H_ */
