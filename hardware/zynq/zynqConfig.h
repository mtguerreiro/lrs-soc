/*
 * zynqConfig.h
 *
 *  Created on: 01.04.2022
 *      Author: Marco Guerreiro
 */

#ifndef ZYNQ_CONFIG_H_
#define ZYNQ_CONFIG_H_

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================
/* Server's port that will be listening for connections */
#define ZYNQ_CONFIG_TCP_SERVER_PORT_OCP				8080
#define ZYNQ_CONFIG_TCP_SERVER_PORT_OPIL            8090

/* Defines if DHCP should be used */
#define ZYNQ_CONFIG_USE_DHCP					1

/*
 * ZYNQ_CONFIG_CPU1_RESET_ADR Defines where CPU 1 looks for a valid address to
 * start execution. ZYNQ_CONFIG_CPU1_START_ADR defines this address. It must
 * match the one in CPU1's linker file.
 * */
#define ZYNQ_CONFIG_CPU1_RESET_ADR 				0xFFFFFFF0
#define ZYNQ_CONFIG_CPU1_START_ADR 				0x10000000

/*
 * Address of sync flag. This address is only used in the very beginning to
 * sync startup. Then, it can be used for other purposes.
 */
#define ZYNQ_CONFIG_CPU0_CPU1_SYNC_FLAG_ADR		0xFFFF0000

/* Software generated interrupts between CPU0 and CPU1 */
#define ZYNQ_CONFIG_SIG_CPU0_TO_CPU1			0x0EU
#define ZYNQ_CONFIG_SIG_CPU1_TO_CPU0			0x0FU

/* CPU0 and CPU1 IDs for SGI */
#define ZYNQ_CONFIG_SIG_CPU0_ID					0x01
#define ZYNQ_CONFIG_SIG_CPU1_ID					0x02

/* Interrupts from PL to CPU1 */
#define ZYNQ_CONFIG_IRQ_PL_TO_CPU1				61U

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
 */
/* OCM memory for CPU0 to CPU1 data */
#define ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_ADR		0xFFFF0000U
#define ZYNQ_CONFIG_MEM_CPU0_TO_CPU1_SIZE		0x4000U

/* OCM memory for CPU1 to CPU0 data */
#define ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_ADR		0xFFFF4000U
#define ZYNQ_CONFIG_MEM_CPU1_TO_CPU0_SIZE		0x4000U

/* OCM memory for PL to CPU1 data */
#define ZYNQ_CONFIG_MEM_PL_TO_CPU1_ADR			0xFFFF8000U
#define ZYNQ_CONFIG_MEM_PL_TO_CPU1_SIZE			0x4000U

/* RAM memory for scope data */
#define ZYNQ_CONFIG_MEM_TRACE_ADR				0x00B00000U
#define ZYNQ_CONFIG_MEM_TRACE_SIZE_MAX			0x02800000U // 0x00A00000U
//-----------------------------------------------------------------------------
//=============================================================================

#endif /* ZYNQ_CONFIG_H_ */
