/*
 * @file wz_tcp_server.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "wz_tcp_server.h"
#include "stdio.h"

#include "wiznet/socket.h"
#include "wiznet/wizchip_conf.h"

#include "ocp/ocp/ocpIf.h"
//=============================================================================

#ifndef DATA_BUF_SIZE
    #define DATA_BUF_SIZE 64
#endif

//===========================================================================
/*-------------------------------- Globals --------------------------------*/
//===========================================================================
static char recvbuf[WZ_TCP_SERVER_RECV_BUF_SIZE];
//===========================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t wzTcpServerRun(uint8_t sn, uint8_t* buf, uint16_t port){

   int32_t ret;
   int32_t size;

	int32_t n;
	int32_t nrx;
	int32_t ntx;
   uint8_t *p;

#if WZ_TCP_SERVER_CFG_DEBUG == 1
   uint8_t destip[4];
   uint16_t destport;
#endif

   switch(getSn_SR(sn))
   {
      case SOCK_ESTABLISHED :
         if(getSn_IR(sn) & Sn_IR_CON)
         {
#if WZ_TCP_SERVER_CFG_DEBUG == 1
            getSn_DIPR(sn, destip);
            destport = getSn_DPORT(sn);

            printf("%d:Connected - %d.%d.%d.%d : %d\r\n",sn, destip[0], destip[1], destip[2], destip[3], destport);
#endif
            setSn_IR(sn,Sn_IR_CON);
         }
         if((getSn_RX_RSR(sn)) > 0) // Don't need to check SOCKERR_BUSY because it doesn't occur.
         {

            /*
            * After receiving a connection, reads the first 4 bytes. They should
            * represent the number of bytes to come next.
            */
            nrx = 0;
            while( nrx < 4 ){
               n = recv(sn, &recvbuf[nrx], 4 - nrx);
               if( n <= 0 ) break;
               nrx += n;
            }
            if( nrx != 4 ){
               close(sn);
               return n;
            } 
            size = *((uint32_t *)recvbuf);

            /* Read the number of expected bytes */
            nrx = 0;
            while( nrx < size ){
               n = recv(sn, &recvbuf[nrx], size - nrx);
               if( n <= 0 ) break;
               nrx += n;
            }
            if( nrx != size ){
               close(sn);
               return n;               
            }

		      /* Calls the interface */
		      p = (uint8_t *)( recvbuf );
		      ret = ocpIf((void *)p, size, (void **)(&p), WZ_TCP_SERVER_RECV_BUF_SIZE);

            /*
            * Now, sends the reply. The reply consists of the command status/size
            * (4 bytes), followed by data (if any).
            */
            n = send(sn, (uint8_t *)(&ret), 4);
            if( n < 4 ){
               close(sn);
               return n;  
            }

            /* Writes data */
            size = ret;
            ntx = 0;
            while( ntx < size ){
               n = send(sn, p, ret);
               if( n <= 0 ) break;
               ntx += n;
            }
            if( ntx != n ){
               printf("Error writing data: %d\r\n", n);
               close(sn);
               return n;               
            }
         }

         //close(sn);
         break;

      case SOCK_CLOSE_WAIT :
#ifdef WZ_TCP_SERVER_CFG_DEBUG
         printf("%d:CloseWait\r\n",sn);
#endif
         if((ret = disconnect(sn)) != SOCK_OK) return ret;
#ifdef WZ_TCP_SERVER_CFG_DEBUG
         printf("%d:Socket Closed\r\n", sn);
#endif
         break;

      case SOCK_INIT :
#ifdef WZ_TCP_SERVER_CFG_DEBUG
         printf("%d:Listen, TCP server loopback, port [%d]\r\n", sn, port);
#endif
         if( (ret = listen(sn)) != SOCK_OK) return ret;
         break;

      case SOCK_CLOSED:
#ifdef WZ_TCP_SERVER_CFG_DEBUG
         printf("%d:TCP server loopback start\r\n",sn);
#endif
         if((ret = socket(sn, Sn_MR_TCP, port, SF_TCP_NODELAY)) != sn) return ret;
#ifdef WZ_TCP_SERVER_CFG_DEBUG
         printf("%d:Socket opened\r\n",sn);
#endif
         break;

      default:
         break;
   }
   return 1;

}
//-----------------------------------------------------------------------------
//=============================================================================

