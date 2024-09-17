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

