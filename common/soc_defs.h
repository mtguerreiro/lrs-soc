/*
 * soc_defs.h
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

#ifndef SOC_DEFS_H_
#define SOC_DEFS_H_

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================

/*
 * CPU1 commands.
 *
 * These commands must start at 0 and increase sequentially.
 */
typedef enum{
	SOC_CMD_CPU0_BLINK = 0,
	SOC_CMD_CPU0_CPU1_BLINK,
	SOC_CMD_CPU0_CPU1_ADC_EN,
	SOC_CMD_CPU0_CPU1_ADC_SPI_FREQ,
	SOC_CMD_CPU0_CPU1_ADC_SAMPLING_FREQ,
	SOC_CMD_CPU0_CPU1_ADC_ERROR_READ,
	SOC_CMD_CPU0_CPU1_ADC_ERROR_CLEAR,
	SOC_CMD_CPU0_TRACE_START,
	SOC_CMD_CPU0_TRACE_READ,
	SOC_CMD_CPU0_TRACE_SIZE_SET,
	SOC_CMD_CPU0_TRACE_SIZE_READ,
	SOC_CMD_CPU0_CONTROL_EN,
	SOC_CMD_CPU0_END
}socCPU0CommandsEnum_t;

/*
 * CPU1 commands.
 *
 * These commands must start at 0 and increase sequentially.
 */
typedef enum{
    SOC_CMD_CPU1_BLINK = 0,
	SOC_CMD_CPU1_ADC_EN,
	SOC_CMD_CPU1_ADC_SPI_FREQ,
	SOC_CMD_CPU1_ADC_SAMPLING_FREQ,
	SOC_CMD_CPU1_ADC_ERROR_READ,
	SOC_CMD_CPU1_ADC_ERROR_CLEAR,
	SOC_CMD_CPU1_TRACE_START,
	SOC_CMD_CPU1_TRACE_SIZE_SET,
	SOC_CMD_CPU1_TRACE_SIZE_READ,
	SOC_CMD_CPU1_CONTROL_EN,
	SOC_CMD_CPU1_END
}socCPU1CommandsEnum_t;

/* Server's port that will be listening for connections */
#define SOC_CONFIG_SERVER_PORT					8080

/*
 * SOC_CPU1_RESET_ADR Defines where CPU 1 looks for a valid address to start
 * execution. SOC_CPU1_START_ADR defines this address. It must match the one
 * in CPU1's linker file.
 * */
#define SOC_CPU1_RESET_ADR 						0xFFFFFFF0
#define SOC_CPU1_START_ADR 						0x10000000

/*
 * Address of sync flag. This address is only used in the very beginning to
 * sync startup. Then, it can be used for other purposes.
 */
#define SOC_CPU0_CPU1_SYNC_FLAG_ADR				0xFFFF0000

/* Software generated interrupts between CPU0 and CPU1 */
#define SOC_SIG_CPU0_TO_CPU1		0x0EU
#define SOC_SIG_CPU1_TO_CPU0		0x0FU

/* Interrupts from PL to CPU1 */
#define SOC_IRQ_PL_TO_CPU1			61U

/* CPU0 and CPU1 IDs for SGI */
#define SOC_SIG_CPU0_ID				0x01
#define SOC_SIG_CPU1_ID				0x02

/* OCM memory for CPU0 to CPU1 data */
#define SOC_MEM_CPU0_TO_CPU1_ADR	0xFFFF0000
#define SOC_MEM_CPU0_TO_CPU1_SIZE	0x4000

/* OCM memory for CPU1 to CPU0 data */
#define SOC_MEM_CPU1_TO_CPU0_ADR	0xFFFF4000
#define SOC_MEM_CPU1_TO_CPU0_SIZE	0x4000

/* OCM memory for PL to CPU1 data */
#define SOC_MEM_PL_TO_CPU1_ADR		0xFFFF8000
#define SOC_MEM_PL_TO_CPU1_SIZE		0x4000

/* RAM memory for scope data */
#define SOC_MEM_TRACE_ADR			0x00B00000
#define SOC_MEM_TRACE_SIZE_MAX			0x00A00000
//#define SOC_MEM_TRACE_SIZE_MAX			(0x00009C40 >> 1)
//=============================================================================

//=============================================================================
/*----------------------- Active front end definitions ----------------------*/
//=============================================================================
/*
 * AFE measurements.
 *
 * +-----+--------------+--------+
 * | ADC |  Measurement | Offset |
 * +-----+--------------+--------+
 * |  1  |  HB current  |    0   |
 * +-----+--------------+--------+
 * |  2  |    DC link   |    2   |
 * +-----+--------------+--------+
 * |  3  | Grid voltage |    4   |
 * +-----+--------------+--------+
 * |  4  | Load current |    6   |
 * +-----+--------------+--------+
 *
 */

#define SOC_ADC_TO_SIGNAL(ADC, GAIN, OFFS) ((float)ADC) * ((float)GAIN) + ((float)OFFS)

#define SOC_AFE_HB_CURRENT					SOC_MEM_PL_TO_CPU1_ADR
#define SOC_AFE_DCLINK						SOC_MEM_PL_TO_CPU1_ADR + 4
#define SOC_AFE_GRID_VOLTAGE				SOC_MEM_PL_TO_CPU1_ADR + 8
#define SOC_AFE_LOAD_CURRENT				SOC_MEM_PL_TO_CPU1_ADR + 12

#define SOC_AFE_HB_CURRENT_SENS_GAIN		(5.0 / 4095.0 / 0.185)
#define SOC_AFE_HB_CURRENT_SENS_OFFS		(-2.5 / 0.185)

#define SOC_AFE_DCLINK_SENS_GAIN			(5.0 / 4095.0 * 900.0 / 23.2)
#define SOC_AFE_DCLINK_SENS_OFFS			(0.0)

#define SOC_AFE_GRID_VOLTAGE_SENS_GAIN		(5.0 / 4095.0 * 2.2 / 150.0 * 1.0 / 2.5 * 5000.0)
#define SOC_AFE_GRID_VOLTAGE_SENS_OFFS		(-2.5 * 2.2 / 150.0 * 1.0 / 2.5 * 5000.0)

#define SOC_AFE_LOAD_CURRENT_SENS_GAIN		(5.0 / 4095.0 * 2.2 / 150.0 * 1000.0)
#define SOC_AFE_LOAD_CURRENT_SENS_OFFS		(-2.5 * 2.2 / 150.0 * 1000.0)

#define SOC_AFE_ERR_DCLINK_OVER				(1 << 0)
#define SOC_AFE_ERR_DCLINK_UNDER			(1 << 1)

#define SOC_AFE_ERR_HB_CURRENT_OVER			(1 << 2)
#define SOC_AFE_ERR_HB_CURRENT_UNDER		(1 << 3)

#define SOC_AFE_ERR_GRID_VOLTAGE_OVER		(1 << 4)
#define SOC_AFE_ERR_GRID_VOLTAGE_UNDER		(1 << 5)

#define SOC_AFE_ERR_LOAD_CURRENT_OVER		(1 << 6)
#define SOC_AFE_ERR_LOAD_CURRENT_UNDER		(1 << 7)

#define SOC_AFE_HB_CURRENT_MAX				15.0
#define SOC_AFE_HB_CURRENT_MIN				-15.0

#define SOC_AFE_DC_LINK_MAX					45.0
#define SOC_AFE_DC_LINK_MIN					5.0
#define SOC_AFE_DC_LINK_PRECHARGE			27.0

#define SOC_AFE_GRID_VOLTAGE_MAX			35.0
#define SOC_AFE_GRID_VOLTAGE_MIN			-35.0

#define SOC_AFE_LOAD_CURRENT_MAX			10.0
#define SOC_AFE_LOAD_CURRENT_MIN			-1.0
//=============================================================================
#endif /* PLAT_DEFS_H_ */
