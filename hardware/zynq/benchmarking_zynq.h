/*
 * benchmarking.h
 *
 *  Created on: May 16, 2023
 *      Author: Wesley Becker
 */

#ifndef SRC_BENCHMARKING_ZYNQ_H_
#define SRC_BENCHMARKING_ZYNQ_H_

#include "stdint.h"

#define BENCHMARKING

#define TICKS_PER_SEC (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2)


uint32_t GetTicks();
int32_t  InitBenchmarking();
uint32_t GetTicks();
float    TicksToNs(uint32_t delta_Ticks);
float TicksToS(uint32_t delta_ticks);
void RestartTimer(void);

#endif /* SRC_BENCHMARKING_ZYNQ_H_ */
