// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
#ifndef __linux__

#include "xstatus.h"
#include "xparameters.h"
#include "xboostcontrol.h"

#define XPAR_XBOOSTCONTROL_NUM_INSTANCES 1

//extern XBoostcontrol_Config XBoostcontrol_ConfigTable[];
XBoostcontrol_Config XBoostcontrol_ConfigTable[XPAR_XBOOSTCONTROL_NUM_INSTANCES] = {
		{.Control_BaseAddress = XPAR_BOOSTCONTROL_0_S_AXI_CONTROL_BASEADDR, .DeviceId = 0}
};

XBoostcontrol_Config *XBoostcontrol_LookupConfig(u16 DeviceId) {
	XBoostcontrol_Config *ConfigPtr = NULL;

	int Index;

	for (Index = 0; Index < XPAR_XBOOSTCONTROL_NUM_INSTANCES; Index++) {
		if (XBoostcontrol_ConfigTable[Index].DeviceId == DeviceId) {
			ConfigPtr = &XBoostcontrol_ConfigTable[Index];
			break;
		}
	}

	return ConfigPtr;
}

int XBoostcontrol_Initialize(XBoostcontrol *InstancePtr, u16 DeviceId) {
	XBoostcontrol_Config *ConfigPtr;

	Xil_AssertNonvoid(InstancePtr != NULL);

	ConfigPtr = XBoostcontrol_LookupConfig(DeviceId);
	if (ConfigPtr == NULL) {
		InstancePtr->IsReady = 0;
		return (XST_DEVICE_NOT_FOUND);
	}

	return XBoostcontrol_CfgInitialize(InstancePtr, ConfigPtr);
}

#endif

