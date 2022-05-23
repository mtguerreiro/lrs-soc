//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
/* Standard */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

/* Tasks */
#include "blink.h"
#include "uiface.h"
#include "ipcomm.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

/* Xilinx includes */
#include "xil_printf.h"
#include "xparameters.h"
#include "netif/xadapter.h"

#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xscugic.h"

//=============================================================================

#define INTC		    XScuGic
#define INTC_DEVICE_ID	XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER	XScuGic_InterruptHandler

#define sev() __asm__("sev")
#define CPU1STARTADR 0xfffffff0
#define COMM_VAL  (*(volatile unsigned long *)(0xFFFF0000))

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
static int mainSysInit(void);
static int  SetupIntrSystem(INTC *IntcInstancePtr);
//
/*
 * Assign the driver structures for the interrupt controller
 */
INTC   IntcInstancePtr;
//=============================================================================

//=============================================================================
/*---------------------------------- Main -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int main(void){

	mainSysInit();

//	sys_thread_new("comm_thrd", (void(*)(void*))comm, 0,
//	                1024,
//	                DEFAULT_THREAD_PRIO);

	sys_thread_new("uiface_thrd", (void(*)(void*))uiface, 0,
	                1024,
	                DEFAULT_THREAD_PRIO);

	sys_thread_new("ipcomm_thrd", (void(*)(void*))ipcomm, (void *)&IntcInstancePtr,
	                1024,
	                DEFAULT_THREAD_PRIO);


	sys_thread_new("blink_thrd", (void(*)(void*))blink, 0,
	                1024,
	                DEFAULT_THREAD_PRIO);

//	xTaskCreate(blink, "blink", BLINK_CONFIG_TASK_STACK_SIZE, NULL, BLINK_CONFIG_TASK_PRIO, NULL);
//	xTaskCreate(comm, "comm", COMM_CONFIG_TASK_STACK_SIZE, NULL, COMM_CONFIG_TASK_PRIO, NULL);

	vTaskStartScheduler();
	while(1);
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int mainSysInit(void){

	int Status;

    //Disable cache on OCM
    Xil_SetTlbAttributes(0xFFFF0000,0x14de2);           // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0


    COMM_VAL = 0;

	// Initialize the SCU Interrupt Distributer (ICD)
	Status = SetupIntrSystem(&IntcInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

    print("CPU0: writing startaddress for cpu1\n\r");
    Xil_Out32(CPU1STARTADR, 0x10000000);
    dmb(); //waits until write has finished

	print("CPU0: Hello World CPU 0\n\r");
	COMM_VAL = 1;

    print("CPU0: sending the SEV to wake up CPU1\n\r");
    sev();

	while(COMM_VAL == 1);

	COMM_VAL = 1;

    return XST_SUCCESS;

}
//-----------------------------------------------------------------------------
static int SetupIntrSystem(INTC *IntcInstancePtr)
{
	int Status;


	XScuGic_Config *IntcConfig;

	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Initialize the  exception table
	 */
	Xil_ExceptionInit();

	/*
	 * Register the interrupt controller handler with the exception table
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 (Xil_ExceptionHandler)INTC_HANDLER,
			 IntcInstancePtr);

	/*
	 * Enable non-critical exceptions
	 */
	Xil_ExceptionEnable();


	return XST_SUCCESS;
}
//-----------------------------------------------------------------------------
//=============================================================================

///*
// * Copyright (C) 2016 - 2019 Xilinx, Inc.
// * All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without modification,
// * are permitted provided that the following conditions are met:
// *
// * 1. Redistributions of source code must retain the above copyright notice,
// *    this list of conditions and the following disclaimer.
// * 2. Redistributions in binary form must reproduce the above copyright notice,
// *    this list of conditions and the following disclaimer in the documentation
// *    and/or other materials provided with the distribution.
// * 3. The name of the author may not be used to endorse or promote products
// *    derived from this software without specific prior written permission.
// *
// * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
// * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
// * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
// * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// * OF SUCH DAMAGE.
// *
// */
//
//#include <stdio.h>
//#include "xparameters.h"
//#include "netif/xadapter.h"
//#include "platform_config.h"
//#include "xil_printf.h"
//
////#include "xparameters.h"
//#include "xgpio.h"
//#include "xil_types.h"
//
//#define LED_ID XPAR_AXI_GPIO_RGB_LED_DEVICE_ID
//#define LED_CHANNEL 1
//#define LED_MASK 0b111111
//
//
//#if LWIP_IPV6==1
//#include "lwip/ip.h"
//#else
//#if LWIP_DHCP==1
//#include "lwip/dhcp.h"
//#endif
//#endif
//
//#ifdef XPS_BOARD_ZCU102
//#ifdef XPAR_XIICPS_0_DEVICE_ID
//int IicPhyReset(void);
//#endif
//#endif
//
//int main_thread();
//void print_echo_app_header();
//void echo_application_thread(void *);
//int blink_thread();
//
//void lwip_init();
//
//#if LWIP_IPV6==0
//#if LWIP_DHCP==1
//extern volatile int dhcp_timoutcntr;
//err_t dhcp_start(struct netif *netif);
//#endif
//#endif
//
//#define THREAD_STACKSIZE 1024
//
//static struct netif server_netif;
//struct netif *echo_netif;
//
//#if LWIP_IPV6==1
//void print_ip6(char *msg, ip_addr_t *ip)
//{
//	print(msg);
//	xil_printf(" %x:%x:%x:%x:%x:%x:%x:%x\n\r",
//			IP6_ADDR_BLOCK1(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK2(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK3(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK4(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK5(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK6(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK7(&ip->u_addr.ip6),
//			IP6_ADDR_BLOCK8(&ip->u_addr.ip6));
//}
//
//#else
//void
//print_ip(char *msg, ip_addr_t *ip)
//{
//	xil_printf(msg);
//	xil_printf("%d.%d.%d.%d\n\r", ip4_addr1(ip), ip4_addr2(ip),
//			ip4_addr3(ip), ip4_addr4(ip));
//}
//
//void
//print_ip_settings(ip_addr_t *ip, ip_addr_t *mask, ip_addr_t *gw)
//{
//
//	print_ip("Board IP: ", ip);
//	print_ip("Netmask : ", mask);
//	print_ip("Gateway : ", gw);
//}
//
//#endif
//int main()
//{
//	sys_thread_new("main_thrd", (void(*)(void*))main_thread, 0,
//	                THREAD_STACKSIZE,
//	                DEFAULT_THREAD_PRIO);
//
//	sys_thread_new("blink_thrd", (void(*)(void*))blink_thread, 0,
//	                THREAD_STACKSIZE,
//	                DEFAULT_THREAD_PRIO);
//	vTaskStartScheduler();
//	while(1);
//	return 0;
//}
//
//void network_thread(void *p)
//{
//    struct netif *netif;
//    /* the mac address of the board. this should be unique per board */
//    unsigned char mac_ethernet_address[] = { 0x00, 0x0a, 0x35, 0x00, 0x01, 0x02 };
//#if LWIP_IPV6==0
//    ip_addr_t ipaddr, netmask, gw;
//#if LWIP_DHCP==1
//    int mscnt = 0;
//#endif
//#endif
//
//    netif = &server_netif;
//
//    xil_printf("\r\n\r\n");
//    xil_printf("-----lwIP Socket Mode Echo server Demo Application ------\r\n");
//
//#if LWIP_IPV6==0
//#if LWIP_DHCP==0
//    /* initialize IP addresses to be used */
//    IP4_ADDR(&ipaddr,  192, 168, 1, 10);
//    IP4_ADDR(&netmask, 255, 255, 255,  0);
//    IP4_ADDR(&gw,      192, 168, 1, 1);
//#endif
//
//    /* print out IP settings of the board */
//
//#if LWIP_DHCP==0
//    print_ip_settings(&ipaddr, &netmask, &gw);
//    /* print all application headers */
//#endif
//
//#if LWIP_DHCP==1
//	ipaddr.addr = 0;
//	gw.addr = 0;
//	netmask.addr = 0;
//#endif
//#endif
//
//#if LWIP_IPV6==0
//    /* Add network interface to the netif_list, and set it as default */
//    if (!xemac_add(netif, &ipaddr, &netmask, &gw, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
//	xil_printf("Error adding N/W interface\r\n");
//	return;
//    }
//#else
//    /* Add network interface to the netif_list, and set it as default */
//    if (!xemac_add(netif, NULL, NULL, NULL, mac_ethernet_address, PLATFORM_EMAC_BASEADDR)) {
//	xil_printf("Error adding N/W interface\r\n");
//	return;
//    }
//
//    netif->ip6_autoconfig_enabled = 1;
//
//    netif_create_ip6_linklocal_address(netif, 1);
//    netif_ip6_addr_set_state(netif, 0, IP6_ADDR_VALID);
//
//    print_ip6("\n\rBoard IPv6 address ", &netif->ip6_addr[0].u_addr.ip6);
//#endif
//
//    netif_set_default(netif);
//
//    /* specify that the network if is up */
//    netif_set_up(netif);
//
//    /* start packet receive thread - required for lwIP operation */
//    sys_thread_new("xemacif_input_thread", (void(*)(void*))xemacif_input_thread, netif,
//            THREAD_STACKSIZE,
//            DEFAULT_THREAD_PRIO);
//
//#if LWIP_IPV6==0
//#if LWIP_DHCP==1
//    dhcp_start(netif);
//    while (1) {
//		vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
//		dhcp_fine_tmr();
//		mscnt += DHCP_FINE_TIMER_MSECS;
//		if (mscnt >= DHCP_COARSE_TIMER_SECS*1000) {
//			dhcp_coarse_tmr();
//			mscnt = 0;
//		}
//	}
//#else
//    xil_printf("\r\n");
//    xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
//    xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");
//
//    print_echo_app_header();
//    xil_printf("\r\n");
//    sys_thread_new("echod", echo_application_thread, 0,
//		THREAD_STACKSIZE,
//		DEFAULT_THREAD_PRIO);
//    vTaskDelete(NULL);
//#endif
//#else
//    print_echo_app_header();
//    xil_printf("\r\n");
//    sys_thread_new("echod",echo_application_thread, 0,
//		THREAD_STACKSIZE,
//		DEFAULT_THREAD_PRIO);
//    vTaskDelete(NULL);
//#endif
//    return;
//}
//
//int main_thread()
//{
//#if LWIP_DHCP==1
//	int mscnt = 0;
//#endif
//
//#ifdef XPS_BOARD_ZCU102
//	IicPhyReset();
//#endif
//
//	/* initialize lwIP before calling sys_thread_new */
//    lwip_init();
//
//    /* any thread using lwIP should be created using sys_thread_new */
//    sys_thread_new("NW_THRD", network_thread, NULL,
//		THREAD_STACKSIZE,
//            DEFAULT_THREAD_PRIO);
//
//#if LWIP_IPV6==0
//#if LWIP_DHCP==1
//    while (1) {
//	vTaskDelay(DHCP_FINE_TIMER_MSECS / portTICK_RATE_MS);
//		if (server_netif.ip_addr.addr) {
//			xil_printf("DHCP request success\r\n");
//			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
//			print_echo_app_header();
//			xil_printf("\r\n");
//			sys_thread_new("echod", echo_application_thread, 0,
//					THREAD_STACKSIZE,
//					DEFAULT_THREAD_PRIO);
//			break;
//		}
//		mscnt += DHCP_FINE_TIMER_MSECS;
//		if (mscnt >= DHCP_COARSE_TIMER_SECS * 2000) {
//			xil_printf("ERROR: DHCP request timed out\r\n");
//			xil_printf("Configuring default IP of 192.168.1.10\r\n");
//			IP4_ADDR(&(server_netif.ip_addr),  192, 168, 1, 10);
//			IP4_ADDR(&(server_netif.netmask), 255, 255, 255,  0);
//			IP4_ADDR(&(server_netif.gw),  192, 168, 1, 1);
//			print_ip_settings(&(server_netif.ip_addr), &(server_netif.netmask), &(server_netif.gw));
//			/* print all application headers */
//			xil_printf("\r\n");
//			xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
//			xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");
//
//			print_echo_app_header();
//			xil_printf("\r\n");
//			sys_thread_new("echod", echo_application_thread, 0,
//					THREAD_STACKSIZE,
//					DEFAULT_THREAD_PRIO);
//			break;
//		}
//	}
//#endif
//#endif
//    vTaskDelete(NULL);
//    return 0;
//}
//
//int blink_thread(){
//	XGpio_Config *cfg_ptr;
//	XGpio led_device;
//	uint32_t data = 0xFF, k;
//
//	//init_platform();
//	//xil_printf("Hello World\n\r");
//	//xil_printf("Successfully ran Hello World application");
//
//	// Initialize LED Device
//	cfg_ptr = XGpio_LookupConfig(LED_ID);
//	XGpio_CfgInitialize(&led_device, cfg_ptr, cfg_ptr->BaseAddress);
//	XGpio_SetDataDirection(&led_device, LED_CHANNEL, 0);
//
//	while( 1 ){
//
//		XGpio_DiscreteWrite(&led_device, LED_CHANNEL, data & LED_MASK);
//
////		k = 10000000;
////		while(k--);
//		vTaskDelay(1000 / portTICK_PERIOD_MS);
//		data = data << 1;
//		if( (data & (LED_MASK << 1 ) ) == 0 ) data = 0xFF;
//	}
//
//
//	//cleanup_platform();
//	return 0;
//}
