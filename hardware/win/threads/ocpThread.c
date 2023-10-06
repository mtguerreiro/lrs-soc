/*
 * ocpThread.c
 *
 *  Created on: 06.10.2023
 *      Author: mguerreiro
 */


//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ocpThread.h"

#include "stdlib.h"
#include "stdio.h"

#include <pthread.h>

#include "winsock2.h"

#include "ocpIf.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
void *opilThreadProcess(void *param);
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
#define OCP_THREAD_SERVER_PORT    8080
#define OCP_THREAD_BUFFER_SIZE    512
#define OCP_THREAD_BACKLOG        5

//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
static WSADATA wsaData;
static SOCKET server_socket = INVALID_SOCKET, client_socket = INVALID_SOCKET;
static struct sockaddr_in server_addr, client_addr;
static int client_len;

//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *ocpThread(void *ptr){

    pthread_t ocpThreadProcessHandle;

    /* Initializes Winsock */
    if( WSAStartup(MAKEWORD(2,2),&wsaData) != 0 ){
        printf("WSAStartup failed. Error Code : %d",WSAGetLastError());
        fflush( stdout );
        return 0;
    }

    /* Creates the server socket */
    if( (server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET ){
        printf("%s: Error creating socket. Error Code : %d", __FUNCTION__, WSAGetLastError());
        fflush( stdout );
        WSACleanup();
        return 0;
    }

    /* Sets up the server address structure */
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(OCP_THREAD_SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    /* Binds the server socket to the address */
    if( bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR ){
        printf("%s: Error binding socket. Error Code : %d", __FUNCTION__, WSAGetLastError());
        fflush( stdout );
        closesocket(server_socket);
        WSACleanup();
        return 0;
    }

    /* Listens for incoming connections */
    if( listen(server_socket, OCP_THREAD_BACKLOG) == SOCKET_ERROR ){
        printf("%s: Error listening for connections. Error Code : %d", __FUNCTION__, WSAGetLastError());
        fflush( stdout );
        closesocket(server_socket);
        WSACleanup();
        return 0;
    }

    printf("%s: OCP interface ready and running.\n", __FUNCTION__);
    fflush( stdout );

    while(1){
        /* Waits for a connection */
        client_len = sizeof(client_addr);
        if( (client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len)) == INVALID_SOCKET ) {
            printf("%s: Error accepting connection. Error Code : %d", __FUNCTION__, WSAGetLastError());
            fflush( stdout );
            closesocket(server_socket);
            WSACleanup();
            return 0;
        }

        printf("%s: Client connected from %s\n", __FUNCTION__, inet_ntoa(client_addr.sin_addr));
        fflush( stdout );
        pthread_create( &ocpThreadProcessHandle, NULL, opilThreadProcess, (void *)&client_socket);
        pthread_join( ocpThreadProcessHandle, NULL);
    }

    fflush( stdout );

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void *opilThreadProcess(void *param){

    int sd = *( (int *)param );
    char recvbuf[OCP_THREAD_BUFFER_SIZE];
    int32_t n;
    int32_t nrx;
    int32_t ret;
    int32_t size;

    int32_t status;

    uint8_t *p;

    /* ADD MUTEX HERE*/

    while(1){
        /*
         * After receiving a connection, reads the first 4 bytes. They should
         * represent the number of bytes to come next.
         */
        nrx = 0;
        while( nrx < 4 ){
          n = recv(sd, (void *)&recvbuf[nrx], 4 - nrx, 0);
          if( n <= 0 ){
              printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
              fflush(stdout);
              printf("recv failed: %d\n", WSAGetLastError());
              fflush(stdout);
              break;
          }
          nrx += n;
        }
        if( nrx != 4 ){
          printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
          fflush(stdout);
          break;
        }
        size = *((uint32_t *)recvbuf);

        /* Read the number of expected bytes */
        nrx = 0;
        while( nrx < size ){
          n = recv(sd, (void *)&recvbuf[nrx], size - nrx, 0);
          if( n <= 0 ){
              printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
              fflush(stdout);
              break;
          }
          nrx += n;
        }
        if( nrx != size ){
          printf("%s: error receiving all expected data from socket %d, closing socket\r\n", __FUNCTION__, sd);
          fflush(stdout);
          break;
        }

        /* Calls the interface */
        p = (uint8_t *)( recvbuf );
        ret = ocpIf((void *)p, size, (void **)(&p), OCP_THREAD_BUFFER_SIZE);

        /*
         * Now, sends the reply. The reply consists of the command status/size
         * (4 bytes), followed by data (if any).
         */
        // TODO: should we also do a while loop to send the data? Like in receiving?

        /* Writes back the command status */
        //*( (int32_t *)recvbuf ) = ret;
        n = send(sd, (void *)&ret, 4, 0);
        if( n < 4 ){
            printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
            fflush(stdout);
            break;
        }

        /* Writes data */
        if( ret > 0 ){
            n = send(sd, (void *)p, ret, 0);
            if( n < ret ){
                printf("%s: error responding to client\'s request\r\n", __FUNCTION__);
                fflush(stdout);
            }
        }
        break;
    }

    /* Closes connection */
    //xSemaphoreGive( xuifaceControl.mutexocp );

    status = shutdown(sd, SD_SEND);
    if( status == SOCKET_ERROR ){
        printf("shutdown failed: %d\n", WSAGetLastError());
        fflush( stdout );
        status = -1;
    }

    closesocket(sd);
    //vTaskDelete(NULL);

    return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================
