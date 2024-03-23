/*
 * ocpCSMaster.h
 *
 *  Created on: 23.03.2023
 *      Author: mguerreiro
 */

#ifndef OCP_CS_MASTER_H_
#define OCP_CS_MASTER_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "../controlsys/controlsys.h"
//============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================


typedef controlsysConfig_t ocpCSMasterConfig_t;

typedef controlsysStatus_t ocpCSMasterStatus_t;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpCSMasterInitialize(uint32_t id, ocpCSMasterConfig_t *config, char *name);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterControllerInterface(uint32_t csid,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterHardwareInterface(uint32_t id,
		void *in, uint32_t insize,
		void **out, uint32_t maxoutsize);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterRun(uint32_t id);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterEnable(uint32_t id);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterDisable(uint32_t id);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterStatus(uint32_t id, int32_t *status);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterGetNumberControllers(void);
//-----------------------------------------------------------------------------
int32_t ocpCSMasterGetControllersNames(char *buffer, int32_t maxsize);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* OCP_CS_MASTER_H_ */