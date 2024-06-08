/*
 * @file ocpPicoCpu0.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpPicoCpu0.h"
#include <stdio.h>
/* Open controller project */
#include "ocp/ocp/ocpTraceMaster.h"
#include "ocp/ocp/ocpCSMaster.h"
#include "ocp/ocp/ocpIfMaster.h"
#include "ocp/ocp/ocpConfig.h"

#include "ocp/hardware/pico/wz_pico_init.h"
#include "ocp/hardware/pico/ipcClientPico.h"

#include "ocp/ipc/ipcClient.h"

#include "ocp/app/itm3903c/itm3903cConfig.h"
#include "ocp/app/itm3903c/itm3903cController.h"
#include "ocp/app/itm3903c/itm3903cHw.h"
#include "ocp/app/itm3903c/itm3903cHwIf.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeHw(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeIpc(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeTraces(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeControlSystem(void);
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeInterface(ocpPicoInitConfig_t *config);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpPicoCpu0Initialize(ocpPicoInitConfig_t *config){

	ocpPicoCpu0InitializeHw();
	ocpPicoCpu0InitializeIpc();
	ocpPicoCpu0InitializeTraces();
	ocpPicoCpu0InitializeControlSystem();
	ocpPicoCpu0InitializeInterface(config);
}
//-----------------------------------------------------------------------------
//=============================================================================


//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeHw(void){

    int32_t status = 0;

	sleep_ms(3000);

    itm3903cHwInitializeC0();
	
    //status = wzPicoInit(0);

    return status;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeIpc(void){

	ipcClientPicoInitialize();

	ipcClientInitialize(ipcClientPicoIrqSend, ipcClientPicoIrqReceive,
		ipcClientPicoLock, ipcClientPicoUnlock,
		OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_ADR, OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_SIZE,
		OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_ADR, OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE);

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeTraces(void){

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeControlSystem(void){

	ocpCSMasterConfig_t config;
	itm3903cControllerConfig_t itconfig;

    /* Initializes controller and hardware interface libs */
	itconfig.disable = 0;
    itconfig.enable = 0;
	itm3903cControllerInitialize(&itconfig);
    itm3903cHwIfInitialize();

    /* Initializes control sys lib */
    config.binputs = (void *)0;
    config.boutputs = (void *)0;

    config.fhwInterface = itm3903cHwDigitalIf;
    config.fhwStatus = itm3903cHwStatus;

    config.fgetInputs = 0;

    config.fapplyOutputs = 0;

    config.frun = 0;
    config.fcontrollerInterface = 0;
    config.fcontrollerStatus = 0;

    config.fenable = 0;
    config.fdisable = 0;

    config.fonEntry = 0;
    config.fonExit = 0;

	ocpCSMasterInitialize(OCP_CS_MASTER_1, &config, "Supply interface");

	return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpPicoCpu0InitializeInterface(ocpPicoInitConfig_t *config){

    ocpIfMasterConfig_t ifConfig;

    ifConfig.lock = config->ifLock;
    ifConfig.unlock = config->ifUnlock;    

	ocpIfMasterInitialize(&ifConfig);

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
