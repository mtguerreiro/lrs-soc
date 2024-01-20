/*
 * @file wz_pico_init.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "wz_pico_init.h"

#include "stdio.h"
#include "pico/sync.h"

#include "wiznet/dhcp.h"
#include "wiznet/socket.h"
#include "wiznet/wizchip_conf.h"
//=============================================================================

//=============================================================================
/*------------------------------- Prototypes --------------------------------*/
//=============================================================================
static void wzPicoInitW5500(void);
static void wzPicoInitW5500DHCP(void);
static void wzPicoInitTimer(void);

static inline void wzPicoW5500ChipSelect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(WZ_PICO_INIT_CFG_SPI_CSN_PIN, 0);  // Active low
    asm volatile("nop \n nop \n nop");
}

static inline void wzPicoW5500ChipDeselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(WZ_PICO_INIT_CFG_SPI_CSN_PIN, 1);
    asm volatile("nop \n nop \n nop");
}

static void wzPicoW5500IPAssignCB(void);
static void wzPicoW5500IPConflictCB(void);

static uint8_t wzPicoSPIRead(void);
static void wzPicoSPIWrite(uint8_t);

static void wzPicoSPIBurstRead(uint8_t *data, uint16_t size);
static void wzPicoSPIBurstWrite(uint8_t *data, uint16_t size);

static void wzPicoInitializeCriticalSection(void);
static void wzPicoCriticalSectionEnter(void);
static void wzPicoCriticalSectionExit(void);

static bool wzPicoDhcpTimer(struct repeating_timer *t);

//=============================================================================

//===========================================================================
/*-------------------------------- Globals --------------------------------*/
//===========================================================================
static wiz_NetInfo gWIZNETINFO = { .mac = {0x00, 0x08, 0xdc,0x00, 0xab, 0xcd},
                            .ip = {192, 168, 0, 231},
                            .sn = {255,255,255,0},
                            .gw = {192, 168, 1, 1},
                            .dns = {0,0,0,0},
                            .dhcp = NETINFO_DHCP };

static critical_section_t crit_sec;

#if WZ_PICO_INIT_CFG_USE_DHCP == 1
static struct repeating_timer timer;
#endif
//===========================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t wzPicoInit(void){

	spi_init(WZ_PICO_INIT_CFG_SPI, WZ_PICO_INIT_CFG_SPI_CLK);
	gpio_set_function(WZ_PICO_INIT_CFG_SPI_SCK_PIN, GPIO_FUNC_SPI);
	gpio_set_function(WZ_PICO_INIT_CFG_SPI_TX_PIN, GPIO_FUNC_SPI);
	gpio_set_function(WZ_PICO_INIT_CFG_SPI_RX_PIN, GPIO_FUNC_SPI);

	gpio_init(WZ_PICO_INIT_CFG_SPI_CSN_PIN);
	gpio_set_dir(WZ_PICO_INIT_CFG_SPI_CSN_PIN, GPIO_OUT);
	gpio_put(WZ_PICO_INIT_CFG_SPI_CSN_PIN, 1);

#if WZ_PICO_INIT_CFG_USE_DHCP == 1
	add_repeating_timer_ms(1000, wzPicoDhcpTimer, NULL, &timer);
#endif

	wzPicoInitializeCriticalSection();

	wzPicoInitW5500();

	return 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*---------------------------- Static functions -----------------------------*/
//=============================================================================

//-----------------------------------------------------------------------------
static void wzPicoInitializeCriticalSection(void){

	critical_section_init(&crit_sec);
}
//-----------------------------------------------------------------------------
static void wzPicoCriticalSectionEnter(void){

	critical_section_enter_blocking(&crit_sec);
}
//-----------------------------------------------------------------------------
static void wzPicoCriticalSectionExit(void){

	critical_section_exit(&crit_sec);
}
//-----------------------------------------------------------------------------
static void wzPicoInitW5500(void){

	reg_wizchip_cris_cbfunc(wzPicoCriticalSectionEnter, wzPicoCriticalSectionExit);
	reg_wizchip_cs_cbfunc(wzPicoW5500ChipSelect, wzPicoW5500ChipDeselect);
	reg_wizchip_spi_cbfunc(wzPicoSPIRead, wzPicoSPIWrite);
	reg_wizchip_spiburst_cbfunc(wzPicoSPIBurstRead, wzPicoSPIBurstWrite);

	wizchip_init(0, 0);

	while(wizphy_getphylink() == PHY_LINK_OFF) sleep_ms(1000);

#if WZ_PICO_INIT_CFG_USE_DHCP == 1
    wzPicoInitW5500DHCP();
#else
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
#endif

#if WZ_PICO_INIT_CFG_DBG == 1
	printf("My IP Address: %d. %d. %d. %d\n", gWIZNETINFO.ip[0], gWIZNETINFO.ip[1], gWIZNETINFO.ip[2], gWIZNETINFO.ip[3]);
	printf("My Subnet Mask: %d. %d. %d. %d\n", gWIZNETINFO.sn[0], gWIZNETINFO.sn[1], gWIZNETINFO.sn[2], gWIZNETINFO.sn[3]);
	printf("My Gateway: %d. %d. %d. %d\n", gWIZNETINFO.gw[0], gWIZNETINFO.gw[1], gWIZNETINFO.gw[2], gWIZNETINFO.gw[3]);
#endif
}
//-----------------------------------------------------------------------------
static void wzPicoInitW5500DHCP(void){

	uint8_t buf[2048];
	uint32_t dhcpStatus;

	setSHAR(gWIZNETINFO.mac);

	DHCP_init(0, buf);
	reg_dhcp_cbfunc(wzPicoW5500IPAssignCB, wzPicoW5500IPAssignCB, wzPicoW5500IPConflictCB);

	while(1){
		dhcpStatus = DHCP_run();

		if(dhcpStatus == DHCP_IP_LEASED) break;

		else if(dhcpStatus == DHCP_FAILED){
			DHCP_stop();
			ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
		}

		sleep_ms(500);
	}
}
//-----------------------------------------------------------------------------
static void wzPicoW5500IPAssignCB(void){

	getIPfromDHCP(gWIZNETINFO.ip);
	getGWfromDHCP(gWIZNETINFO.gw);
	getSNfromDHCP(gWIZNETINFO.sn);
	getDNSfromDHCP(gWIZNETINFO.dns);
	gWIZNETINFO.dhcp = NETINFO_DHCP;

	/* Network initialization */
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
}
//-----------------------------------------------------------------------------
static void wzPicoW5500IPConflictCB(void){
#if WZ_PICO_INIT_CFG_DBG == 1
	printf("CONFLICT IP from DHCP\r\n");
#endif
	//halt or reset or any...
	while(1); // this example is halt.
}
//-----------------------------------------------------------------------------
static uint8_t wzPicoSPIRead(void){

	uint8_t data;

	spi_read_blocking(spi_default, 0xFF, &data, 1);

	return data;
}
//-----------------------------------------------------------------------------
static void wzPicoSPIWrite(uint8_t data){

	spi_write_blocking(spi_default, &data, 1);
}
//-----------------------------------------------------------------------------
static void wzPicoSPIBurstRead(uint8_t *data, uint16_t size){

	spi_read_blocking(spi_default, 0xFF, data, size);
}
//-----------------------------------------------------------------------------
static void wzPicoSPIBurstWrite(uint8_t *data, uint16_t size){

	spi_write_blocking(spi_default, data, size);
}
//-----------------------------------------------------------------------------
static bool wzPicoDhcpTimer(struct repeating_timer *t){

	DHCP_time_handler();

	return true;
}
//-----------------------------------------------------------------------------
//=============================================================================
