/*
 * @file ipcClientPico.c
 *
 * @brief Client for the inter-processor communication (IPC) library.
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "ipcClientPico.h"

/* Drivers */
#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "pico/multicore.h"

/* Configs */
#include "ocp/hardware/pico/ocpPicoConfig.h"
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientPicoIrq(void);
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{

	/*
	 * Semaphore used to signal a response from CPU1.
	 *
	 * This semaphore is taken before a command is issued to CPU1. When CPU1
	 * replies back, an interrupt is generated in CPU0, which releases the
	 * semaphore. Thus, the semaphore is used to indicate that CPU1 replied
	 * to a command issued by CPU0.
	 */
	volatile int32_t semaphore;

	/*
	 * Semaphore used to lock/unlock the client.
	 *
	 * A request is only sent to the server if the semaphore is free. The
	 * semaphore is freed after receiving the server's response.
	 */
	int32_t lock;

}ipcClientPicoControl_t;
//=============================================================================

//=============================================================================
/*--------------------------------- Globals ---------------------------------*/
//=============================================================================
ipcClientPicoControl_t xipcClientPicoControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientPicoInitialize(void){

	multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_IRQ_PROC0, ipcClientPicoIrq);
    irq_set_enabled(SIO_IRQ_PROC0, true);

    xipcClientPicoControl.lock = 0;
    xipcClientPicoControl.semaphore = 0;
}
//-----------------------------------------------------------------------------
int32_t ipcClientPicoIrqSend(void){

    xipcClientPicoControl.semaphore = 1;

    multicore_fifo_push_blocking(0x01);

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ipcClientPicoIrqReceive(uint32_t timeout){

    while( xipcClientPicoControl.semaphore != 0 ){
	}

	return 0;
}
//-----------------------------------------------------------------------------
int32_t ipcClientPicoLock(uint32_t timeout){

    if( xipcClientPicoControl.lock != 0 ) return IPC_CLIENT_PICO_ERR_LOCK_TO;

    xipcClientPicoControl.lock = 1;

    return 0;
}
//-----------------------------------------------------------------------------
void ipcClientPicoUnlock(void){

    xipcClientPicoControl.lock = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------------- IRQ -----------------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
void ipcClientPicoIrq(void){

    multicore_fifo_pop_blocking();
    multicore_fifo_clear_irq();

    xipcClientPicoControl.semaphore = 0;
}
//-----------------------------------------------------------------------------
//=============================================================================