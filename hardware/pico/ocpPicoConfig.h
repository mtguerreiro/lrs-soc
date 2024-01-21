/*
 * picoConfig.h
 *
 */

#ifndef OCP_PICO_CONFIG_H_
#define OCP_PICO_CONFIG_H_

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* Server's port that will be listening for connections */
#define OCP_PICO_CONFIG_TCP_SERVER_PORT_OCP			8080
// #define PICO_CONFIG_TCP_SERVER_PORT_OPIL            8090

/* If DHCP is set to 1, DHCP is used. Otherwise, static IP */
#define OCP_PICO_CONFIG_WZ_USE_DHCP		            0

/* Memory for CPU0 to CPU1 data */
#define OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_ADR		0x20004000
#define OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE		256

/* Memory for CPU1 to CPU0 data */
#define OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_ADR		(OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_ADR + OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE)
#define OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_SIZE		OCP_PICO_CONFIG_MEM_CPU0_TO_CPU1_SIZE

/* RAM for scope data */
#define OCP_PICO_CONFIG_MEM_TRACE_ADR				(OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_ADR + OCP_PICO_CONFIG_MEM_CPU1_TO_CPU0_SIZE)
#define OCP_PICO_CONFIG_MEM_TRACE_SIZE_MAX			(10 * 1024)

/* SPI configs */
#define OCP_PICO_CONFIG_WZ_SPI			            PICO_DEFAULT_SPI_INSTANCE	
#define OCP_PICO_CONFIG_WZ_SPI_CLK		            10 * 1000 * 1000 /* Clock is in Hz */
#define OCP_PICO_CONFIG_WZ_SPI_SCK_PIN	            PICO_DEFAULT_SPI_SCK_PIN
#define OCP_PICO_CONFIG_WZ_SPI_TX_PIN	            PICO_DEFAULT_SPI_TX_PIN
#define OCP_PICO_CONFIG_WZ_SPI_RX_PIN  	            PICO_DEFAULT_SPI_RX_PIN
#define OCP_PICO_CONFIG_WZ_SPI_CSN_PIN	            PICO_DEFAULT_SPI_CSN_PIN

/* LED config */
#define OCP_PICO_CONFIG_LED_PIN		                PICO_DEFAULT_LED_PIN
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* OCP_PICO_CONFIG_H_ */
