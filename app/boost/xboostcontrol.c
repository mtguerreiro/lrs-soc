// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
/***************************** Include Files *********************************/
#include "xboostcontrol.h"

/************************** Function Implementation *************************/
#ifndef __linux__
int XBoostcontrol_CfgInitialize(XBoostcontrol *InstancePtr, XBoostcontrol_Config *ConfigPtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(ConfigPtr != NULL);

    InstancePtr->Control_BaseAddress = ConfigPtr->Control_BaseAddress;
    InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

    return XST_SUCCESS;
}
#endif

void XBoostcontrol_Start(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL) & 0x80;
    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL, Data | 0x01);
}

u32 XBoostcontrol_IsDone(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL);
    return (Data >> 1) & 0x1;
}

u32 XBoostcontrol_IsIdle(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL);
    return (Data >> 2) & 0x1;
}

u32 XBoostcontrol_IsReady(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL);
    // check ap_start to see if the pcore is ready for next input
    return !(Data & 0x1);
}

void XBoostcontrol_EnableAutoRestart(XBoostcontrol *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL, 0x80);
}

void XBoostcontrol_DisableAutoRestart(XBoostcontrol *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_AP_CTRL, 0);
}

void XBoostcontrol_Set_Li(XBoostcontrol *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_LI_DATA, Data);
}

u32 XBoostcontrol_Get_Li(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_LI_DATA);
    return Data;
}

void XBoostcontrol_Set_min_v_in(XBoostcontrol *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_MIN_V_IN_DATA, Data);
}

u32 XBoostcontrol_Get_min_v_in(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_MIN_V_IN_DATA);
    return Data;
}

void XBoostcontrol_Set_min_v_out(XBoostcontrol *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_MIN_V_OUT_DATA, Data);
}

u32 XBoostcontrol_Get_min_v_out(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_MIN_V_OUT_DATA);
    return Data;
}

void XBoostcontrol_Set_adc_gain_v_in_inv(XBoostcontrol *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_IN_INV_DATA, Data);
}

u32 XBoostcontrol_Get_adc_gain_v_in_inv(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_IN_INV_DATA);
    return Data;
}

void XBoostcontrol_Set_adc_gain_v_out_inv(XBoostcontrol *InstancePtr, u32 Data) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_OUT_INV_DATA, Data);
}

u32 XBoostcontrol_Get_adc_gain_v_out_inv(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_OUT_INV_DATA);
    return Data;
}

u32 XBoostcontrol_Get_D_debug(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_D_DEBUG_DATA);
    return Data;
}

u32 XBoostcontrol_Get_D_debug_vld(XBoostcontrol *InstancePtr) {
    u32 Data;

    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Data = XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_D_DEBUG_CTRL);
    return Data & 0x1;
}

void XBoostcontrol_InterruptGlobalEnable(XBoostcontrol *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_GIE, 1);
}

void XBoostcontrol_InterruptGlobalDisable(XBoostcontrol *InstancePtr) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_GIE, 0);
}

void XBoostcontrol_InterruptEnable(XBoostcontrol *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_IER);
    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_IER, Register | Mask);
}

void XBoostcontrol_InterruptDisable(XBoostcontrol *InstancePtr, u32 Mask) {
    u32 Register;

    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    Register =  XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_IER);
    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_IER, Register & (~Mask));
}

void XBoostcontrol_InterruptClear(XBoostcontrol *InstancePtr, u32 Mask) {
    Xil_AssertVoid(InstancePtr != NULL);
    Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    XBoostcontrol_WriteReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ISR, Mask);
}

u32 XBoostcontrol_InterruptGetEnabled(XBoostcontrol *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_IER);
}

u32 XBoostcontrol_InterruptGetStatus(XBoostcontrol *InstancePtr) {
    Xil_AssertNonvoid(InstancePtr != NULL);
    Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

    return XBoostcontrol_ReadReg(InstancePtr->Control_BaseAddress, XBOOSTCONTROL_CONTROL_ADDR_ISR);
}

