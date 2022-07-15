/*
 * uiface.c
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "uiface.h"

/* Kernel */
#include "FreeRTOS.h"
#include "task.h"

/* Device and drivers */
#include "xparameters.h"
#include "netif/xadapter.h"
//#include "platform_config.h"
#include "xil_printf.h"
#include "xil_types.h"

/* Network settings */
#if LWIP_IPV6==1
#include "lwip/ip.h"
#else
#if UIFACE_CONFIG_USE_DHCP==1
#include "lwip/dhcp.h"
#endif
#endif

/* lwip */
#include "lwip/sockets.h"
#include "lwipopts.h"
//=============================================================================

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================
#define UIFACE_ERR_INVALID_ID						-1

#define UIFACE_CONFIG_SERVER_PORT					SOC_CONFIG_SERVER_PORT
#define UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT		1024
#define UIFACE_CONFIG_THREAD_PRIO_DEFAULT			DEFAULT_THREAD_PRIO

/* Ticks to wait to process new request */
#define UIFACE_CONFIG_MUTEX_WAIT					UIFACE_CONFIG_MUTEX_WAIT_MS / portTICK_PERIOD_MS

/* Ethernet settings */
#define UIFACE_PLAT_EMAC_BASEADDR					XPAR_XEMACPS_0_BASEADDR

typedef struct{

	/* Handles for received commands */
	uifaceHandle_t handle[SOC_CMD_CPU0_END];

	/* Server netif */
	struct netif servernetif;

	/* Mutex to ensure one request from the UI is processed at a time */
	SemaphoreHandle_t mutex;
}uifaceControl_t;
//=============================================================================


//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
uifaceControl_t xuifaceControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static void uifaceApplicationThread(void);

/**
 * @brief Thread spawned for each connection.
 *
 * @param void *p Socket index.
 */
static void uifaceRequestProcessThread(void *p);

/**
 * @brief Initializes socket and updates DHCP timer.
 */
static void uifaceNetworkThread(void *p);

/**
 * @brief Prints the specified IP4.
 * @param msg Message to print before printing the IP.
 * @param ip IP to be printed.
 */
static void uifacePrintIP(char *msg, ip_addr_t *ip);

/**
 * @brief Prints IP settings
 */
static void uifacePrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw);

#if LWIP_IPV6==0
#if UIFACE_CONFIG_USE_DHCP==1
extern volatile int dhcp_timoutcntr;
err_t dhcp_start(struct netif *netif);
#endif
#endif
void lwip_init();
//=============================================================================

//=============================================================================
/*---------------------------------- Task -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void uiface(void *param){

	struct netif *netif;

#if UIFACE_CONFIG_USE_DHCP==1
	int mscnt = 0;
#endif

	xuifaceControl.mutex = xSemaphoreCreateMutex();

	/* initialize lwIP before calling sys_thread_new */
    lwip_init();

    /* any thread using lwIP should be created using sys_thread_new */
    sys_thread_new("uifNWThread", uifaceNetworkThread, NULL,
    		UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
			UIFACE_CONFIG_THREAD_PRIO_DEFAULT);

    netif = &xuifaceControl.servernetif;

#if LWIP_IPV6==0
#if UIFACE_CONFIG_USE_DHCP==1
    while (1) {

    	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);

    	if (netif->ip_addr.addr) {
			xil_printf("DHCP request success\r\n");
			uifacePrintIPSettings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
			xil_printf("\r\n");
			sys_thread_new("uifAppThread", (void(*)(void*))uifaceApplicationThread, 0,
					UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
					UIFACE_CONFIG_THREAD_PRIO_DEFAULT);
			break;
		}

		mscnt += DHCP_FINE_TIMER_MSECS;

		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
			xil_printf("ERROR: DHCP request timed out\r\n");
			xil_printf("Configuring default IP of 192.168.1.10\r\n");
			IP4_ADDR(&(netif->ip_addr),  192, 168, 1, 10);
			IP4_ADDR(&(netif->netmask), 255, 255, 255,  0);
			IP4_ADDR(&(netif->gw),  192, 168, 1, 1);
			uifacePrintIPSettings(&(netif->ip_addr), &(netif->netmask), &(netif->gw));
			/* print all application headers */
			xil_printf("\r\n");
			xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
			xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

			xil_printf("\r\n");
			sys_thread_new("uifAppThread", (void(*)(void*))uifaceApplicationThread, 0,
					UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
					UIFACE_CONFIG_THREAD_PRIO_DEFAULT);
			break;
		}
	}
#endif
#endif
    vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int32_t uifaceRegisterHandle(uint32_t id, uifaceHandle_t handle){

	if( id >= SOC_CMD_CPU0_END ) return UIFACE_ERR_INVALID_ID;

	xuifaceControl.handle[id] = handle;

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static void uifaceApplicationThread(void){

	int sock, new_sd;
	int size;
#if LWIP_IPV6==0
	struct sockaddr_in address, remote;

	memset(&address, 0, sizeof(address));

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	address.sin_family = AF_INET;
	address.sin_port = htons(UIFACE_CONFIG_SERVER_PORT);
	address.sin_addr.s_addr = INADDR_ANY;
#else
	struct sockaddr_in6 address, remote;

	memset(&address, 0, sizeof(address));

	address.sin6_len = sizeof(address);
	address.sin6_family = AF_INET6;
	address.sin6_port = htons(echo_port);

	memset(&(address.sin6_addr), 0, sizeof(address.sin6_addr));

	if ((sock = lwip_socket(AF_INET6, SOCK_STREAM, 0)) < 0)
		return;
#endif

	if (lwip_bind(sock, (struct sockaddr *)&address, sizeof (address)) < 0)
		return;

	lwip_listen(sock, 0);

	size = sizeof(remote);

	while (1) {
		if ((new_sd = lwip_accept(sock, (struct sockaddr *)&remote, (socklen_t *)&size)) > 0) {
			xil_printf("%s: received connection from", __FUNCTION__);
			uifacePrintIP(": ", (ip_addr_t *)&remote.sin_addr);
			sys_thread_new("uifProcRecThread", uifaceRequestProcessThread,
				(void*)new_sd,
				UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
				UIFACE_CONFIG_THREAD_PRIO_DEFAULT);
		}
	}
}
//-----------------------------------------------------------------------------
static void uifaceRequestProcessThread(void *p){

	int sd = (int)p;
	char recvbuf[UIFACE_CONFIG_RECV_BUFFER];
	int32_t n;
	int32_t nrx;
	uint32_t id;
	uifaceDataExchange_t dataExchange;
	int32_t ret;
	int32_t *ptr;
	int32_t size;

	/*
	 * A mutex is added here so that we can ensure that only one request is
	 * processed at a time. This is important to prevent writes to any shared
	 * memory that may being read.
	 */
	if( xSemaphoreTake( xuifaceControl.mutex, UIFACE_CONFIG_MUTEX_WAIT ) == pdFALSE ){
		/* Can't process the request, so closes the connection */
		xil_printf("%s: Couldn't take mutex, closing socket\r\n", __FUNCTION__);
		lwip_close(sd);
		vTaskDelete(NULL);
	}

	while (1) {

		/*
		 * After receiving a connection, reads the first 8 bytes. They should
		 * the command ID (4 bytes) and the number of bytes that were sent
		 * (also 4 bytes).
		 */
		nrx = 0;
		while( nrx < 8 ){
			n = lwip_read(sd, &recvbuf[nrx], 8 - nrx);
			if( n <= 0 ){
				xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
				break;
			}
			nrx += n;
		}
		if( nrx != 8 ){
			xil_printf("%s: error reading ID and size from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}

		id = *((uint32_t *)recvbuf);
		if( id >= SOC_CMD_CPU0_END ){
			xil_printf("%s: bad id (%u), closing socket\r\n", __FUNCTION__, id);
			break;
		}

		size = *((uint32_t *)&recvbuf[4]);
		if( size > UIFACE_CONFIG_RECV_BUFFER ){
			xil_printf("%s: data sent exceeds buffer size (command %u).\r\n", __FUNCTION__, id);
			xil_printf("%s: Number of bytes sent: %u\t Buffer size: %u\r\n", __FUNCTION__, size, UIFACE_CONFIG_RECV_BUFFER);
			xil_printf("%s: closing socket.\r\n", __FUNCTION__);
			break;
		}

		/*
		 * If everything checks out, proceed to read the number of expected
		 * bytes.
		 */
		nrx = 0;
		while( nrx < size ){
			n = lwip_read(sd, &recvbuf[nrx], size - nrx);
			if( n <= 0 ){
				xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
				break;
			}
			nrx += n;
		}
		if( nrx != size ){
			xil_printf("%s: error receiving all expected data from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}

		/* Calls function registered to the received ID */
		dataExchange.cmd = id;
		dataExchange.buffer = (uint8_t *)recvbuf;
		dataExchange.size = (uint32_t)(size);
		if( xuifaceControl.handle[id] == 0 ){
			xil_printf("%s: no handle for id %u, closing socket\r\n", __FUNCTION__, id);
			break;
		}
		ret = xuifaceControl.handle[id](&dataExchange);

		/*
		 * Now, sends the reply. The reply consists of the command status
		 * (4 bytes), followed by the number of bytes to be sent (4 bytes)
		 * and the actual data.
		 */
		// TODO: should we also do a while loop to send the data? Like in receiving?

		/* Writes back command status */
		ptr = (int32_t *)recvbuf;
		if( ret >= 0 ){
			*ptr = (int32_t)UFIACE_STATUS_CMD_EXEC_PASS;
		}
		else{
			*ptr = ret;
		}
		n = lwip_write(sd, recvbuf, 4);
		if( n < 4 ){
			xil_printf("%s: error responding to client request (id %u)\r\n", __FUNCTION__, id);
			break;
		}

		/* Writes back number of bytes to be sent */
		ptr = (int32_t *)recvbuf;
		if( ret > 0 ){
			*ptr = (int32_t)dataExchange.size;
		}
		else{
			*ptr = 0;
		}
		n = lwip_write(sd, recvbuf, 4);
		if( n < 4 ){
			xil_printf("%s: error responding to client request (id %u)\r\n", __FUNCTION__, id);
			break;
		}

		/* Writes data */
		if( ret > 0 ){
			n = lwip_write(sd, dataExchange.buffer, dataExchange.size);
			if( n < dataExchange.size ) xil_printf("%s: error responding to client request (id %u)\r\n", __FUNCTION__, id);
		}
		break;
	}

	/* Closes connection */
	xSemaphoreGive( xuifaceControl.mutex );
	lwip_close(sd);
	vTaskDelete(NULL);
}
//-----------------------------------------------------------------------------
static void uifaceNetworkThread(void *p){
    struct netif *netif;
    /* the mac address of the board. this should be unique per board */
    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
#if LWIP_IPV6==0
    ip_addr_t ipaddr, netmask, gw;
#if UIFACE_CONFIG_USE_DHCP==1
    int mscnt = 0;
#endif
#endif

    netif = &xuifaceControl.servernetif;

    xil_printf("\r\n\r\n");
    xil_printf("----- LRS-SoC controller - initializing network settings ------\r\n");

#if LWIP_IPV6==0
#if UIFACE_CONFIG_USE_DHCP==0
    /* initialize IP addresses to be used */
    IP4_ADDR(&ipaddr,  192, 168, 1, 10);
    IP4_ADDR(&netmask, 255, 255, 255,  0);
    IP4_ADDR(&gw,      192, 168, 1, 1);
#endif

    /* print out IP settings of the board */

#if UIFACE_CONFIG_USE_DHCP==0
    print_ip_settings(&ipaddr, &netmask, &gw);
    /* print all application headers */
#endif

#if UIFACE_CONFIG_USE_DHCP==1
	ipaddr.addr = 0;
	gw.addr = 0;
	netmask.addr = 0;
#endif
#endif

#if LWIP_IPV6==0
    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, UIFACE_PLAT_EMAC_BASEADDR)) {
	xil_printf("Error adding N/W interface\r\n");
	return;
    }
#else
    /* Add network interface to the netif_list, and set it as default */
    if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address, UIFACE_PLAT_EMAC_BASEADDR)) {
	xil_printf("Error adding N/W interface\r\n");
	return;
    }

    netif->ip6_autoconfig_enabled = 1;

    netif_create_ip6_linklocal_address(netif, 1);
    netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);

    print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);
#endif

    netif_set_default(netif);

    /* specify that the network if is up */
    netif_set_up(netif);

    /* start packet receive thread - required for lwIP operation */
    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
    		UIFACE_CONFIG_THREAD_STACK_SIZE_DEFAULT,
			UIFACE_CONFIG_THREAD_PRIO_DEFAULT);

#if LWIP_IPV6==0
#if UIFACE_CONFIG_USE_DHCP==1
    dhcp_start(netif);
    while (1) {
		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
		dhcp_fine_tmr();
		mscnt += DHCP_FINE_TIMER_MSECS;
		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
			dhcp_coarse_tmr();
			mscnt = 0;
		}
	}
#else
    xil_printf("\r\n");
    xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
    xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

    print_echo_app_header();
    xil_printf("\r\n");
    sys_thread_new("echod", echo_application_thread, 0,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);
    vTaskDelete(NULL);
#endif
#else
    print_echo_app_header();
    xil_printf("\r\n");
    sys_thread_new("echod",echo_application_thread, 0,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);
    vTaskDelete(NULL);
#endif
    return;
}
//-----------------------------------------------------------------------------
static void uifacePrintIP(char *msg, ip_addr_t *ip){

	xil_printf(msg);
	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
			ip4_addr3(ip), ip4_addr4(ip));
}
//-----------------------------------------------------------------------------
static void uifacePrintIPSettings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw){

	uifacePrintIP("Board IP: ", ip);
	uifacePrintIP("Netmask : ", mask);
	uifacePrintIP("Gateway : ", gw);
}
//-----------------------------------------------------------------------------
//=============================================================================
