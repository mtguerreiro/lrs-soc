
#ifdef SOC_CPU1
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpZynqCpu1.h"

/* Benchmarking */
#include "benchmarking_zynq.h"

/* Inter-processor communication */
#include "../ipc/ipcServer.h"

/* Zynq-specific stuff */
#include "zynqConfig.h"
#include "ipcServerZynq.h"

/* Open controller project */
#include "ocpIf.h"

/* Application */
//#include "fsbuckboost.h"
#include "cuk.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeHw(void *intcInst);
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_ADDR        ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_SIZE        ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE

#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_ADDR        ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR
#define OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_SIZE        ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ocpZynqCpu1Initialize(void *intcInst){

    /* Initialize timer for benchmarking */
    InitBenchmarking();

    ocpZynqCpu1InitializeIpc(intcInst);
    ocpZynqCpu1InitializeHw(intcInst);

    ocpIfInitialize();
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeIpc(void *intcInst){

    /* Initializes inter-processor communication */
    ipcServerZynqInitialize(intcInst);

    ipcServerInitialize(ocpIf, ipcServerZynqIrqSend,
            OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_ADDR, OCP_ZYNQ_C1_CONFIG_CPU0_TO_CPU1_SIZE,
            OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_ADDR, OCP_ZYNQ_C1_CONFIG_CPU1_TO_CPU0_SIZE);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t ocpZynqCpu1InitializeHw(void *intcInst){

    fsbuckboostInit(intcInst);
    //cukInit(intcInst);

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* SOC_CPU1 */
