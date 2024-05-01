
//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpTcpProc.h"

#include "ocpIfMaster.h"
#if OCP_TCP_PROC_CFG_DEBUG == 1
#include "stdio.h"
#endif
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static ocpTcpProcRead_t tcpRead;
static ocpTcpProcWrite_t tcpWrite;

static uint8_t recvbuf[OCP_TCP_PROC_BUF_SIZE];

//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t ocpTcpProcInitialize(ocpTcpProcConfig_t *config){

    tcpRead = config->tcpRead;
    tcpWrite = config->tcpWrite;

    return 0;
}
//-----------------------------------------------------------------------------
int32_t ocpTcpProc(uint32_t sn){

    int32_t ret;
    int32_t size;

    int32_t n;
    int32_t nrx;
    int32_t ntx;
    uint8_t *p;

    /*
    * After receiving a connection, reads the first 4 bytes. They should
    * represent the number of bytes to come next.
    */
    nrx = 0;
    while( nrx < 4 ){
        n = tcpRead(sn, &recvbuf[nrx], 4 - nrx, 0, 0);
        if( n <= 0 ) break;
        nrx += n;
    }

    if( nrx != 4 ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s: Error receiving size. Expected 4 bytes, received %d.\r\n", __func__, nrx);
#endif
        return -1;
    } 

    size = *((uint32_t *)recvbuf);
    if( size == 0 ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s: Received size is 0, something strange happened...\r\n", __func__);
#endif
        return -1;
    }

    /* Read the number of expected bytes */
    if( size > OCP_TCP_PROC_BUF_SIZE ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s Error receiving data. Expected %d bytes, but size of buffer is %d. \r\n", __func__, size, OCP_TCP_PROC_BUF_SIZE);
#endif
        return n;
    }

    nrx = 0;
    while( nrx < size ){
        n = tcpRead(sn, &recvbuf[nrx], size - nrx, 0, 0);
        if( n <= 0 ) break;
        nrx += n;
    }
    if( nrx != size ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s Error receiving data. Expected %d bytes, received %d. \r\n", __func__, size, nrx);
#endif
        return n;
    }

    /* Calls the interface */
    p = recvbuf;
    ret = ocpIfMaster((void *)p, size, (void **)(&p), OCP_TCP_PROC_BUF_SIZE - 4);

    /*
    * Now, sends the reply. The reply consists of the command status/size
    * (4 bytes), followed by data (if any).
    */
    n = tcpWrite(sn, (uint8_t *)(&ret), 4, 0, 0);
    if( n < 4 ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s Error sending size.\r\n", __func__);
#endif
        return -1;  
    }

    /* Writes data */
    size = ret;
    ntx = 0;
    while( ntx < size ){
        n = tcpWrite(sn, p, ret, 0, 0);
        if( n < 0 ) break;
        ntx += n;
        p += n;
    }

    if( ntx != size ){
#if OCP_TCP_PROC_CFG_DEBUG == 1
        printf("%s Error sending data. Should send %d but sent only %d.\r\n", __func__, size, ntx);
#endif
        return -1;
    }

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================