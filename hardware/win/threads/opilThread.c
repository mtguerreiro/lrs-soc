/*
 * opilThread.c
 *
 *  Created on: 06.10.2023
 *      Author: mguerreiro
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "opilThread.h"

#include "stdlib.h"
#include "stdio.h"

#include "opiltarget.h"

#include "ctlrif/ctlrif.h"
#include "comm/win/targetCommWinSock.h"

#include "../ocpWin.h"
#include "cukOpil.h"

//#include "../buckcontrol.h"

#include "ocpIf.h"

#include "config/stypes.h"

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t opilOcpWinUpdateMeasurements(void *meas, int32_t size);
static int32_t opilOcpWinUpdateSimData(void *simData, int32_t size);
void opilOcpWinRunControl(void);
static int32_t opilOcpWinGetControl(void **control);
static int32_t opilOcpWinGetControllerData(void **controllerData);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
//static stypesMeasurements_t xtMeasurements;
//static stypesSimData_t xtSimData;
//static stypesControl_t xtControl;
//static stypesControllerData_t xtControllerData;

static uint8_t txbuf[256];
static uint8_t rxbuf[256];
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *opilThread(void *ptr){

    int32_t conn;

    /* Initializes target */
    //ctlrifInitialize(buckcontrolInitialize, buckcontrol);

    /* Initializes opil */
    opiltargetCommConfig_t comm;
    opiltargetControlConfig_t control;

    comm.openConn = targetCommWinSockOpenConnection;
    comm.closeConn = targetCommWinSockCloseConnection;
    comm.sendData = targetCommWinSockSendData;
    comm.receiveData = targetCommWinSockReceiveData;

    control.updateMeas = opilOcpWinUpdateMeasurements;//ctlrifUpdateMeasurements;
    control.updateSimData = opilOcpWinUpdateSimData;//ctlrifUpdateSimData;

    control.initControl = ctlrifInitializeControl;
    control.runControl = opilOcpWinRunControl;//ctlrifRunControl;

    control.getControl = opilOcpWinGetControl;//ctlrifGetControl;
    control.getControllerData = opilOcpWinGetControllerData;//ctlrifGetControllerData;

    opiltargetInitialize(&comm, &control);

    /* Runs opil target */

    printf("%s: OPiL target module running...\n", __FUNCTION__);
    fflush( stdout );

    while( 1 ){

    printf("%s: Waiting for a connection...\n", __FUNCTION__);
    fflush( stdout );
    conn = opiltargetConnectToHost(0);

    if( conn != 0 ){
        printf("%s: Didn't receive a proper connection, trying it again...\n", __FUNCTION__);
        fflush( stdout );
        continue;
    }

    printf("%s: Connection received! Executing controller...\n", __FUNCTION__);
    fflush( stdout );

    while( opiltargetExchangeDataHost() == 0 );

    printf("%s: Connection terminated! Starting over\n", __FUNCTION__);
    fflush( stdout );

    opiltargetDisconnectFromHost(0);

    }

    printf("%s: \nClosing OPiL target module.\n", __FUNCTION__);
    fflush( stdout );

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t opilOcpWinUpdateMeasurements(void *meas, int32_t size){

    int32_t status;
    int32_t *p;
    uint32_t k;

    uint8_t *dst, *src;

    k = size;
    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_UPDATE_MEASUREMENTS;

    dst = &txbuf[4];
    src = (uint8_t *)meas;
    for(k = 0; k < size; k++) *dst++ = *src++;

    status = ocpIf( (void *)&txbuf, size + 4, 0, 0 );
    //status = ipcClientRequest( (void *)&txbuf, size + 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
    if( status < 0 ) return status;

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t opilOcpWinUpdateSimData(void *simData, int32_t size){

    int32_t status;
    int32_t *p;
    uint32_t k;

    uint8_t *dst, *src;

    k = size;
    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_UPDATE_SIM_DATA;

    dst = &txbuf[4];
    src = (uint8_t *)simData;
    for(k = 0; k < size; k++) *dst++ = *src++;

    status = ocpIf( (void *)&txbuf, size + 4, 0, 0 );
    //status = ipcClientRequest( (void *)&txbuf, size + 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
    if( status < 0 ) return status;

    return 0;
}
//-----------------------------------------------------------------------------
void opilOcpWinRunControl(void){

    int32_t *p;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_RUN_CONTROL;

    ocpIf( (void *)&txbuf, 4, 0, 0 );
    //ipcClientRequest( (void *)&txbuf, 4, 0, 0, OPIL_ZYNQ_CPU0_REQ_TO );
}
//-----------------------------------------------------------------------------
static int32_t opilOcpWinGetControl(void **control){

    //void **vp = 0;
    int32_t size;
    int32_t *p;
    //uint32_t k;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_GET_CONTROL;

    //vp = (void *)&xtControl;

    size = ocpIf( (void *)&txbuf, 4, control, sizeof(stypesControl_t) );

    //opilOcpWinCopy(*vp, *control, size);

    //size = ipcClientRequest( (void *)&txbuf, 4, (void **)&vp, sizeof(stypesControl_t), OPIL_ZYNQ_CPU0_REQ_TO );

    //*control = *vp;

    return size;
}
//-----------------------------------------------------------------------------
static int32_t opilOcpWinGetControllerData(void **controllerData){

    int32_t size;
    int32_t *p;

    p = (int32_t *)txbuf;
    *p = OCP_IF_CMD_OPIL_GET_CONTROLLER_DATA;

    size = ocpIf( (void *)&txbuf, 4, controllerData, sizeof(stypesControllerData_t) );
    //size = ipcClientRequest( (void *)&txbuf, 4, (void **)&xtControllerData, sizeof(stypesControllerData_t), OPIL_ZYNQ_CPU0_REQ_TO );

    //*controllerData = &xtControllerData;

    return size;
}
//-----------------------------------------------------------------------------
//=============================================================================
