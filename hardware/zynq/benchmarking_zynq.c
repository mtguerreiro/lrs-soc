
#include "benchmarking_zynq.h"
#include "xparameters.h"
#include "xscutimer.h"

// Configuration
XScuTimer_Config *pcfg_scu_timer;
XScuTimer         scu_timer;

int32_t InitBenchmarking()
{
	int error = -1;

	// config
	pcfg_scu_timer = XScuTimer_LookupConfig(XPAR_PS7_SCUTIMER_0_DEVICE_ID);
	error = XScuTimer_CfgInitialize(&scu_timer, pcfg_scu_timer, XPAR_PS7_SCUTIMER_0_BASEADDR);

	if(error)
	{
		return error;
	}

	XScuTimer_LoadTimer(&scu_timer, 0xFFFFFFFF);
	XScuTimer_EnableAutoReload(&scu_timer);

    XScuTimer_Start(&scu_timer);

	return error;
}

// Returns the count value from the CPU's private timer
uint32_t GetTicks()
{
	return XScuTimer_GetCounterValue(&scu_timer);
}
// Converts the given number of ticks to seconds
float TicksToS(uint32_t delta_ticks)
{
	return (float)delta_ticks/TICKS_PER_SEC; // s
}

// Converts the given number of ticks to nanoseconds
float TicksToNs(uint32_t delta_ticks)
{
	return (float)delta_ticks/TICKS_PER_SEC * 1.0e9; // ns
}

void RestartTimer(void){

	XScuTimer_Stop(&scu_timer);
	XScuTimer_LoadTimer(&scu_timer, 0xFFFFFFFF);
	XScuTimer_EnableAutoReload(&scu_timer);

    XScuTimer_Start(&scu_timer);
}
