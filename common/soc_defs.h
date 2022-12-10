/*
 * soc_defs.h
 *
 *  Created on: 23.05.2022
 *      Author: Marco Guerreiro
 */

#ifndef SOC_DEFS_H_
#define SOC_DEFS_H_

#ifdef SOC_CPU0
#include "lwipopts.h"
#endif

//=============================================================================
/*--------------------------------- Defines ---------------------------------*/
//=============================================================================

/*
 * CPU1 commands.
 *
 * These commands must start at 0 and increase sequentially.
 */
typedef enum{
	SOC_CMD_CPU0_BLINK_CPU0 = 0,
	SOC_CMD_CPU0_BLINK_CPU1,
	SOC_CMD_CPU0_ADC_EN,
	SOC_CMD_CPU0_ADC_SPI_FREQ_SET,
	SOC_CMD_CPU0_ADC_SAMPLING_FREQ_SET,
	SOC_CMD_CPU0_ADC_ERROR_READ,
	SOC_CMD_CPU0_ADC_ERROR_CLEAR,
	SOC_CMD_CPU0_TRACE_START,
	SOC_CMD_CPU0_TRACE_READ,
	SOC_CMD_CPU0_TRACE_READ_TAGS,
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
	SOC_CMD_CPU1_ADC_SPI_FREQ_SET,
	SOC_CMD_CPU1_ADC_SAMPLING_FREQ_SET,
	SOC_CMD_CPU1_ADC_ERROR_READ,
	SOC_CMD_CPU1_ADC_ERROR_CLEAR,
	SOC_CMD_CPU1_TRACE_START,
	SOC_CMD_CPU1_TRACE_READ,
	SOC_CMD_CPU1_TRACE_READ_TAGS,
	SOC_CMD_CPU1_TRACE_SIZE_SET,
	SOC_CMD_CPU1_TRACE_SIZE_READ,
	SOC_CMD_CPU1_CONTROL_EN,
	SOC_CMD_CPU1_END
}socCPU1CommandsEnum_t;

/* Server's port that will be listening for connections */
#define SOC_CONFIG_SERVER_PORT			8080

/*
 * SOC_CPU1_RESET_ADR Defines where CPU 1 looks for a valid address to start
 * execution. SOC_CPU1_START_ADR defines this address. It must match the one
 * in CPU1's linker file.
 * */
#define SOC_CPU1_RESET_ADR 				0xFFFFFFF0
#define SOC_CPU1_START_ADR 				0x10000000

/*
 * Address of sync flag. This address is only used in the very beginning to
 * sync startup. Then, it can be used for other purposes.
 */
#define SOC_CPU0_CPU1_SYNC_FLAG_ADR		0xFFFF0000

/* Software generated interrupts between CPU0 and CPU1 */
#define SOC_SIG_CPU0_TO_CPU1				0x0EU
#define SOC_SIG_CPU1_TO_CPU0				0x0FU

/* Interrupts from PL to CPU1 */
#define SOC_IRQ_PL_TO_CPU1					61U

/* CPU0 and CPU1 IDs for SGI */
#define SOC_SIG_CPU0_ID						0x01
#define SOC_SIG_CPU1_ID						0x02

//-----------------------------------------------------------------------------
/*
 * Next, memory definitions are given.
 *
 * It should be noted that CPU0, CPU1 and the PL can exchange data either by
 * OCM or RAM.
 *
 * The RAM has its own segmentation. In our project, we reserved a block of
 * RAM for CPU0, a block of RAM for CPU1 and the remaining for the trace
 * buffer. In the following, whenever we mention RAM, we refer to the block
 * reserved for the trace buffer.
 *
 * Currently, the following topology is used:
 *
 * - CPU0 has read/write access to OCM and can read the trace buffer.
 * - CPU1 has read/write access access to OCM and can write to the trace
 * 	 buffer.
 * - The PL has write access to OCM.
 *
 * Data exchange between all parts is as follows. CPU0 and CPU1 exchange data
 * through OCM only. A block of OCM is reserved for CPU0->CPU1 data; CPU0 can
 * only write to this block, and CPU1 can only read from this block. Likewise,
 * there is a block reserved for CPU1->CPU0 data; CPU0 can only read from this
 * block and CPU1 can only write to it.
 *
 * To effectively exchange data between both CPUs, each OCM block is further
 * segmented as follows. For now, CPU0 always send a command to CPU1, which
 * then replies. CPU1 never initiates a transaction. To this end, the
 * CPU0->CPU1 is arranged as follows:
 *
 * CP0->CPU1 memory arrangement
 * +--------------------------------+-----------+------------------------------------+
 * |           ADDRESS				|   DATA    |       ADDRESS SYMBOL		         |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU0_TO_CPU1_ADR		|   CMD     | SOC_MEM_CPU0_TO_CPU1_CMD	         |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU0_TO_CPU1_ADR + 4	|   SIZE    | SOC_MEM_CPU0_TO_CPU1_CMD_SIZE 	 |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU0_TO_CPU1_ADR + 8	| DATA ADDR | SOC_MEM_CPU0_TO_CPU1_CMD_DATA_ADDR |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU0_TO_CPU1_ADR + 12	|   DATA    | SOC_MEM_CPU0_TO_CPU1_DATA 		 |
 * +--------------------------------+-----------+------------------------------------+
 *
 * 	- The first four bytes represent the command to be sent to CPU1. This is
 * 	  defined by the SOC_MEM_CPU0_TO_CPU1_CMD symbol.
 *
 * 	- The next four bytes represent the amount of data to be read by CPU1.
 * 	  This is defined by the SOC_MEM_CPU0_TO_CPU1_CMD_SIZE symbol.
 *
 * 	- The next four bytes contain the address of where the data to be
 * 	  transfered to CPU1 is located. It could be on the OCM itself or
 * 	  elsewhere (for instance, RAM). It is important that this address is
 * 	  accessible by CPU1.
 *
 * 	- The next N bytes can be used to hold the actual data to be sent to
 * 	  CPU0. The starting address is represented by the
 * 	  SOC_MEM_CPU0_TO_CPU1_DATA symbol.
 *
 * Thus, when CPU0 wants to execute a command on CPU1, it must write the
 * to the CPU0->CPU1 memory as above.
 *
 * After executing the command, CPU1 needs to reply back to CPU0. To this
 * end, the CPU1->CPU0 buffer is segmented as follows:
 *
 * CP1->CPU0 memory arrangement
 * +--------------------------------+-----------+------------------------------------+
 * |           ADDRESS				|   DATA    |       ADDRESS SYMBOL		         |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU1_TO_CPU0_ADR   	|  STATUS   | SOC_MEM_CPU1_TO_CPU0_CMD_STATUS    |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU1_TO_CPU0_ADR + 4	| DATA ADDR | SOC_MEM_CPU1_TO_CPU0_CMD_DATA_ADDR |
 * +--------------------------------+-----------+------------------------------------+
 * | SOC_MEM_CPU1_TO_CPU0_ADR + 8	|   DATA    | SOC_MEM_CPU1_TO_CPU0_DATA 		 |
 * +--------------------------------+-----------+------------------------------------+
 *
 * 	- First four bytes are the command status. This is a numeric value that
 * 	  is a negative value if the command was not executed properly, or a
 * 	  value larger than or equal to 0 if the command was successful. If the
 * 	  value is larger than 0, then it represents how many bytes of data CPU1
 * 	  wants to send to CPU0. This address is also defined by the symbol
 * 	  SOC_MEM_CPU1_TO_CPU0_CMD_STATUS
 *
 * 	- The next four bytes contain the address of where the data to be
 * 	  transfered to CPU0 is located. It could be on the OCM itself or
 * 	  elsewhere (for instance, RAM). It is important that this address is
 * 	  accessible by CPU0. This address is defined by the symbol
 * 	  SOC_MEM_CPU1_TO_CPU0_CMD_DATA_ADDR.
 *
 * 	- The next N bytes can be used to hold the actual data to be sent to
 * 	  CPU0. The starting address is represented by the
 * 	  SOC_MEM_CPU1_TO_CPU0_DATA symbol.
 *
 * Thus, when CPU1 replies to CPU0, it must write the to the CPU1->CPU0
 * memory as above.
 *
 */
/* OCM memory for CPU0 to CPU1 data */
#define SOC_MEM_CPU0_TO_CPU1_ADR				0xFFFF0000U
#define SOC_MEM_CPU0_TO_CPU1_SIZE				(0x4000U - 12U)
#define SOC_MEM_CPU0_TO_CPU1_CMD_SIZE			(SOC_MEM_CPU0_TO_CPU1_ADR)
#define SOC_MEM_CPU0_TO_CPU1_CMD				(SOC_MEM_CPU0_TO_CPU1_ADR + 4U)
//#define SOC_MEM_CPU0_TO_CPU1_CMD_DATA_ADDR		(SOC_MEM_CPU0_TO_CPU1_ADR + 8U)
#define SOC_MEM_CPU0_TO_CPU1_DATA				(SOC_MEM_CPU0_TO_CPU1_ADR + 8U)

/* OCM memory for CPU1 to CPU0 data */
#define SOC_MEM_CPU1_TO_CPU0_ADR				0xFFFF4000U
#define SOC_MEM_CPU1_TO_CPU0_SIZE				(0x4000U - 8U)
#define SOC_MEM_CPU1_TO_CPU0_CMD_STATUS			SOC_MEM_CPU1_TO_CPU0_ADR
#define SOC_MEM_CPU1_TO_CPU0_CMD_DATA_ADDR		(SOC_MEM_CPU1_TO_CPU0_ADR + 4U)
#define SOC_MEM_CPU1_TO_CPU0_CMD_DATA			(SOC_MEM_CPU1_TO_CPU0_ADR + 8U)

/* OCM memory for PL to CPU1 data */
#define SOC_MEM_PL_TO_CPU1_ADR					0xFFFF8000U
#define SOC_MEM_PL_TO_CPU1_SIZE					0x4000U

/* RAM memory for scope data */
#define SOC_MEM_TRACE_ADR						0x00B00000U
#define SOC_MEM_TRACE_SIZE_MAX					0x00A00000U
//-----------------------------------------------------------------------------

#define	SOC_UIFACE_USE_DHCP					LWIP_DHCP
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

#define SOC_AFE_HB_CURRENT_MAX				25.0
#define SOC_AFE_HB_CURRENT_MIN				-25.0

#define SOC_AFE_DC_LINK_MAX					300.0
#define SOC_AFE_DC_LINK_MIN					2.0
#define SOC_AFE_DC_LINK_PRECHARGE			8.0

#define SOC_AFE_GRID_VOLTAGE_MAX			35.0
#define SOC_AFE_GRID_VOLTAGE_MIN			-35.0

#define SOC_AFE_LOAD_CURRENT_MAX			10.0
#define SOC_AFE_LOAD_CURRENT_MIN			-1.0
//=============================================================================
#endif /* PLAT_DEFS_H_ */
