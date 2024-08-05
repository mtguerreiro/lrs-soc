// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef XBOOSTCONTROL_H
#define XBOOSTCONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#ifndef __linux__
#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xil_io.h"
#else
#include <stdint.h>
#include <assert.h>
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stddef.h>
#endif
#include "xboostcontrol_hw.h"

/**************************** Type Definitions ******************************/
#ifdef __linux__
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
#else
typedef struct {
    u16 DeviceId;
    u64 Control_BaseAddress;
} XBoostcontrol_Config;
#endif

typedef struct {
    u64 Control_BaseAddress;
    u32 IsReady;
} XBoostcontrol;

typedef u32 word_type;

/***************** Macros (Inline Functions) Definitions *********************/
#ifndef __linux__
#define XBoostcontrol_WriteReg(BaseAddress, RegOffset, Data) \
    Xil_Out32((BaseAddress) + (RegOffset), (u32)(Data))
#define XBoostcontrol_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))
#else
#define XBoostcontrol_WriteReg(BaseAddress, RegOffset, Data) \
    *(volatile u32*)((BaseAddress) + (RegOffset)) = (u32)(Data)
#define XBoostcontrol_ReadReg(BaseAddress, RegOffset) \
    *(volatile u32*)((BaseAddress) + (RegOffset))

#define Xil_AssertVoid(expr)    assert(expr)
#define Xil_AssertNonvoid(expr) assert(expr)

#define XST_SUCCESS             0
#define XST_DEVICE_NOT_FOUND    2
#define XST_OPEN_DEVICE_FAILED  3
#define XIL_COMPONENT_IS_READY  1
#endif

/************************** Function Prototypes *****************************/
#ifndef __linux__
int XBoostcontrol_Initialize(XBoostcontrol *InstancePtr, u16 DeviceId);
XBoostcontrol_Config* XBoostcontrol_LookupConfig(u16 DeviceId);
int XBoostcontrol_CfgInitialize(XBoostcontrol *InstancePtr, XBoostcontrol_Config *ConfigPtr);
#else
int XBoostcontrol_Initialize(XBoostcontrol *InstancePtr, const char* InstanceName);
int XBoostcontrol_Release(XBoostcontrol *InstancePtr);
#endif

void XBoostcontrol_Start(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_IsDone(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_IsIdle(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_IsReady(XBoostcontrol *InstancePtr);
void XBoostcontrol_EnableAutoRestart(XBoostcontrol *InstancePtr);
void XBoostcontrol_DisableAutoRestart(XBoostcontrol *InstancePtr);

void XBoostcontrol_Set_Li(XBoostcontrol *InstancePtr, u32 Data);
u32 XBoostcontrol_Get_Li(XBoostcontrol *InstancePtr);
void XBoostcontrol_Set_min_v_in(XBoostcontrol *InstancePtr, u32 Data);
u32 XBoostcontrol_Get_min_v_in(XBoostcontrol *InstancePtr);
void XBoostcontrol_Set_min_v_out(XBoostcontrol *InstancePtr, u32 Data);
u32 XBoostcontrol_Get_min_v_out(XBoostcontrol *InstancePtr);
void XBoostcontrol_Set_adc_gain_v_in_inv(XBoostcontrol *InstancePtr, u32 Data);
u32 XBoostcontrol_Get_adc_gain_v_in_inv(XBoostcontrol *InstancePtr);
void XBoostcontrol_Set_adc_gain_v_out_inv(XBoostcontrol *InstancePtr, u32 Data);
u32 XBoostcontrol_Get_adc_gain_v_out_inv(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_Get_D_debug(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_Get_D_debug_vld(XBoostcontrol *InstancePtr);

void XBoostcontrol_InterruptGlobalEnable(XBoostcontrol *InstancePtr);
void XBoostcontrol_InterruptGlobalDisable(XBoostcontrol *InstancePtr);
void XBoostcontrol_InterruptEnable(XBoostcontrol *InstancePtr, u32 Mask);
void XBoostcontrol_InterruptDisable(XBoostcontrol *InstancePtr, u32 Mask);
void XBoostcontrol_InterruptClear(XBoostcontrol *InstancePtr, u32 Mask);
u32 XBoostcontrol_InterruptGetEnabled(XBoostcontrol *InstancePtr);
u32 XBoostcontrol_InterruptGetStatus(XBoostcontrol *InstancePtr);

#ifdef __cplusplus
}
#endif

#endif
