// ==============================================================
// Vitis HLS - High-Level Synthesis from C, C++ and OpenCL v2022.2 (64-bit)
// Tool Version Limit: 2019.12
// Copyright 1986-2022 Xilinx, Inc. All Rights Reserved.
// ==============================================================
// control
// 0x00 : reserved
// 0x04 : reserved
// 0x08 : reserved
// 0x0c : reserved
// 0x10 : Data signal of Li
//        bit 31~0 - Li[31:0] (Read/Write)
// 0x14 : reserved
// 0x18 : Data signal of min_v_in
//        bit 31~0 - min_v_in[31:0] (Read/Write)
// 0x1c : reserved
// 0x20 : Data signal of min_v_out
//        bit 31~0 - min_v_out[31:0] (Read/Write)
// 0x24 : reserved
// 0x28 : Data signal of adc_gain_v_in_inv
//        bit 31~0 - adc_gain_v_in_inv[31:0] (Read/Write)
// 0x2c : reserved
// 0x30 : Data signal of adc_gain_v_out_inv
//        bit 31~0 - adc_gain_v_out_inv[31:0] (Read/Write)
// 0x34 : reserved
// (SC = Self Clear, COR = Clear on Read, TOW = Toggle on Write, COH = Clear on Handshake)

#define XBOOSTCONTROL_CONTROL_ADDR_LI_DATA                 0x10
#define XBOOSTCONTROL_CONTROL_BITS_LI_DATA                 32
#define XBOOSTCONTROL_CONTROL_ADDR_MIN_V_IN_DATA           0x18
#define XBOOSTCONTROL_CONTROL_BITS_MIN_V_IN_DATA           32
#define XBOOSTCONTROL_CONTROL_ADDR_MIN_V_OUT_DATA          0x20
#define XBOOSTCONTROL_CONTROL_BITS_MIN_V_OUT_DATA          32
#define XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_IN_INV_DATA  0x28
#define XBOOSTCONTROL_CONTROL_BITS_ADC_GAIN_V_IN_INV_DATA  32
#define XBOOSTCONTROL_CONTROL_ADDR_ADC_GAIN_V_OUT_INV_DATA 0x30
#define XBOOSTCONTROL_CONTROL_BITS_ADC_GAIN_V_OUT_INV_DATA 32

