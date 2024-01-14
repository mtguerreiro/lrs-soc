/*
 * @file wz_pico_init.h
 *
 */

#ifndef WZ_PICO_INIT_H_
#define WZ_PICO_INIT_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"

#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "ocp/hardware/pico/ocpPicoConfig.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

/* SPI configs */
#define WZ_PICO_INIT_CFG_SPI			    OCP_PICO_CONFIG_WZ_SPI	
#define WZ_PICO_INIT_CFG_SPI_CLK		    OCP_PICO_CONFIG_WZ_SPI_CLK
#define WZ_PICO_INIT_CFG_SPI_SCK_PIN	    OCP_PICO_CONFIG_WZ_SPI_SCK_PIN
#define WZ_PICO_INIT_CFG_SPI_TX_PIN	        OCP_PICO_CONFIG_WZ_SPI_TX_PIN
#define WZ_PICO_INIT_CFG_SPI_RX_PIN	        OCP_PICO_CONFIG_WZ_SPI_RX_PIN
#define WZ_PICO_INIT_CFG_SPI_CSN_PIN	    OCP_PICO_CONFIG_WZ_SPI_CSN_PIN

/* If DHCP is set to 1, DHCP is used. Otherwise, static IP */
#define WZ_PICO_INIT_CFG_USE_DHCP		    OCP_PICO_CONFIG_WZ_USE_DHCP

/* Prints debugging info if enabled */
#define WZ_PICO_INIT_CFG_DBG                1
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
int32_t wzPicoInit(void);
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* WZ_PICO_INIT_H_ */
