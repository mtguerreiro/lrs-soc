/*
 * itm3903cHwIf.c
 *
 */

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "itm3903cHwIf.h"
#include <stdio.h>
#include <stdbool.h>

#include "itm3903cConfig.h"

#include "ocp/utils/rp.h"

#include "itm3903cHw.h"
#include "string.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

typedef struct{
    rphandle_t handles[ITM3903C_HW_DIGITAL_IF_END];
    rpctx_t rp;
}itm3903cHwDigitalIf_t;

typedef struct{
    rphandle_t handles[ITM3903C_HW_ANALOG_IF_END];
    rpctx_t rp;
}itm3903cHwAnalogIf_t;

typedef struct{

    /*
     * The digital interface uses RS232 to communicate with the source, and is
     * used to configure the supply.
     */
    itm3903cHwDigitalIf_t digitalIf;

    /*
     * The analog interface has access to the hardware that generates the 
     * ADC/DAC system that acquire analog measurements and drive the analog
     * inputs of the supply.
     */
    itm3903cHwAnalogIf_t analogIf;
}itm3903cHwIfControl_t;

static itm3903cHwIfControl_t hwControl;
//=============================================================================

//=============================================================================
/*-------------------------------- Prototypes -------------------------------*/
//=============================================================================
/* Digital interface */
static int32_t itm3903cHwIfSetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfClearError(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetCurrValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t itm3903cHwIfGetVoltageMeasurement(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentMeasurement(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltageMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltageMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetPowerMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetPowerMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetPowerMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetPowerMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetCurrentMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetCurrentMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageUnderProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltageProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetPowerProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetPowerProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentUnderProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetCurrentProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageSlewRate(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetVoltageSlewRate(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t itm3903cHwIfGetVoltageOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetVoltageUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetCurrentUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetPowerOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetPowerUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
/* Analog interface */
static int32_t itm3903cHwIfSetSamplingStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetSamplingStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetSamplingFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetSamplingFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetDacA1Offset(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetDacA1Adj(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetDacA2(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetDacA3(void *in, uint32_t insize, void **out, uint32_t maxoutsize);

static int32_t itm3903cHwIfSetDacCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetAdcCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetDacCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetAdcCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfSetAlphaValues(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
static int32_t itm3903cHwIfGetAlphaValues(void *in, uint32_t insize, void **out, uint32_t maxoutsize);
//=============================================================================

//=============================================================================
/*-------------------------------- Functions --------------------------------*/
//=============================================================================
int32_t itm3903cHwIfInitialize(void){

    /* Initializes the request processor of the digital interface */
    rpInitialize(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_END, hwControl.digitalIf.handles);

    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_SLOPE, itm3903cHwIfSetSlope);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_SLOPE, itm3903cHwIfGetSlope);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_OFFSET, itm3903cHwIfSetOffset);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_OFFSET, itm3903cHwIfGetOffset);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VERSION, itm3903cHwIfGetVersion);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_ERROR, itm3903cHwIfGetError);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_CLEAR_ERROR, itm3903cHwIfClearError);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_OUTPUT_STATUS, itm3903cHwIfSetOutputStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_FUNC_MODE, itm3903cHwIfSetFuncMode);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_FUNC_MODE, itm3903cHwIfGetFuncMode);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_OUTPUT_STATUS, itm3903cHwIfGetOutputStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfSetAnalogExternalStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_ANALOG_EXTERNAL_STATUS, itm3903cHwIfGetAnalogExternalStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLT_VALUE, itm3903cHwIfSetVoltValue);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_CURR_VALUE, itm3903cHwIfSetCurrValue);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MEASUREMENT, itm3903cHwIfGetVoltageMeasurement);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MEASUREMENT, itm3903cHwIfGetCurrentMeasurement);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MAX, itm3903cHwIfGetVoltageMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_MIN, itm3903cHwIfGetVoltageMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_MAX, itm3903cHwIfSetVoltageMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_MIN, itm3903cHwIfSetVoltageMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_POWER_MAX, itm3903cHwIfGetPowerMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_POWER_MIN, itm3903cHwIfGetPowerMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_POWER_MAX, itm3903cHwIfSetPowerMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_POWER_MIN, itm3903cHwIfSetPowerMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MAX, itm3903cHwIfGetCurrentMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_MIN, itm3903cHwIfGetCurrentMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_CURRENT_MAX, itm3903cHwIfSetCurrentMax);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_CURRENT_MIN, itm3903cHwIfSetCurrentMin);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_PROTECTION_STATUS, itm3903cHwIfGetVoltageProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_UNDER_PROTECTION_STATUS, itm3903cHwIfGetVoltageUnderProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_PROTECTION_STATUS, itm3903cHwIfSetVoltageProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_POWER_PROTECTION_STATUS, itm3903cHwIfGetPowerProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_POWER_PROTECTION_STATUS, itm3903cHwIfSetPowerProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_PROTECTION_STATUS, itm3903cHwIfGetCurrentProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_UNDER_PROTECTION_STATUS, itm3903cHwIfGetCurrentUnderProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_CURRENT_PROTECTION_STATUS, itm3903cHwIfSetCurrentProtectionStatus);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_SLOW_RATE, itm3903cHwIfGetVoltageSlewRate);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_SET_VOLTAGE_SLOW_RATE, itm3903cHwIfSetVoltageSlewRate);

    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_OVERPROTECTION_VALUE,itm3903cHwIfGetVoltageOverProtection);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_VOLTAGE_UNDERPROTECTION_VALUE,itm3903cHwIfGetVoltageUnderProtection);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_OVERPROTECTION_VALUE,itm3903cHwIfGetCurrentOverProtection);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_CURRENT_UNDERPROTECTION_VALUE,itm3903cHwIfGetCurrentUnderProtection);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_POWER_OVERPROTECTION_VALUE,itm3903cHwIfGetPowerOverProtection);
    rpRegisterHandle(&hwControl.digitalIf.rp, ITM3903C_HW_DIGITAL_IF_GET_POWER_UNDERPROTECTION_VALUE,itm3903cHwIfGetPowerUnderProtection);

    /* Initializes the request processor of the analog interface */
    rpInitialize(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_END, hwControl.analogIf.handles);

    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_SAMPLING_STATUS, itm3903cHwIfSetSamplingStatus);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_GET_SAMPLING_STATUS, itm3903cHwIfGetSamplingStatus);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_SAMPLING_FREQ, itm3903cHwIfSetSamplingFreq);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_GET_SAMPLING_FREQ, itm3903cHwIfGetSamplingFreq);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_DAC_A1_OFFSET, itm3903cHwIfSetDacA1Offset);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_DAC_A1_ADJ, itm3903cHwIfSetDacA1Adj);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_DAC_A2, itm3903cHwIfSetDacA2);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_DAC_A3, itm3903cHwIfSetDacA3);

    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_DAC_CAL_DATA, itm3903cHwIfSetDacCalData);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_GET_DAC_CAL_DATA, itm3903cHwIfGetDacCalData);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_ADC_CAL_DATA, itm3903cHwIfSetAdcCalData);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_GET_ADC_CAL_DATA, itm3903cHwIfGetAdcCalData);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_GET_ALPHA_VALUES, itm3903cHwIfGetAlphaValues);
    rpRegisterHandle(&hwControl.analogIf.rp, ITM3903C_HW_ANALOG_IF_SET_ALPHA_VALUES, itm3903cHwIfSetAlphaValues);

    return 0;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwDigitalIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.digitalIf.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
int32_t itm3903cHwAnalogIf(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    int32_t status;

    status = rpRequest(&hwControl.analogIf.rp, in, insize, out, maxoutsize);

    return status;
}
//-----------------------------------------------------------------------------
//=============================================================================

//=============================================================================
/*----------------------------- Static functions ----------------------------*/
//=============================================================================
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float slope;

    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    slope = *( (float *)p );

    itm3903cHwSetSlope(channel, slope);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetSlope(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float slope;
    float *o = (float *)*out;
    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    slope = itm3903cHwGetSlope(channel);

    *o = slope;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float offset;

    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    offset = *( (float *)p );

    itm3903cHwSetOffset(channel, offset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetOffset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t channel;
    float offset;
    float *o = (float *)*out;
    uint32_t *p = (uint32_t *)in;

    channel = *p++;

    offset = itm3903cHwGetOffset(channel);

    *o = offset;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVersion(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetVersion(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetFuncMode(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    char *o = (char *)*out;
    
    return itm3903cHwGetError(o, maxoutsize);
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfClearError(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    itm3903cHwClearError();
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetOutputStatus(setStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetFuncMode(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t funcMode = *p;
    
    itm3903cHwSetFuncMode(funcMode);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setStatus = *p;
    
    itm3903cHwSetAnalogExternalStatus(setStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetOutputStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *)*out;

    uint32_t output_status = itm3903cHwGetOutputStatus();

    *o = output_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetAnalogExternalStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t output_status = itm3903cHwGetAnalogExternalStatus();

    *o = output_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float volt_value;

    uint32_t *p = (uint32_t *)in;

    volt_value = *( (float *)p );

    itm3903cHwSetValue(volt_value, true);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetCurrValue(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float curr_value;

    uint32_t *p = (uint32_t *)in;

    curr_value = *( (float *)p );

    itm3903cHwSetValue(curr_value, false);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageMeasurement(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float voltage_measurement;
    float *o = (float *)*out;

    voltage_measurement = itm3903cHwGetVoltageMeasurement();

    *o = voltage_measurement;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentMeasurement(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float current_measurement;
    float *o = (float *)*out;

    current_measurement = itm3903cHwGetCurrentMeasurement();

    *o = current_measurement;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float voltage_max;
    float *o = (float *)*out;

    voltage_max = itm3903cHwGetVoltageMax();
 
    *o = voltage_max;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float voltage_min;
    float *o = (float *)*out;

    voltage_min = itm3903cHwGetVoltageMin();

    *o = voltage_min;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltageMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float volt_max;

    uint32_t *p = (uint32_t *)in;

    volt_max = *( (float *)p );

    itm3903cHwSetVoltageMax(volt_max);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltageMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float volt_min;

    uint32_t *p = (uint32_t *)in;

    volt_min = *( (float *)p );

    itm3903cHwSetVoltageMin(volt_min);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetPowerMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float power_max;
    float *o = (float *)*out;

    power_max = itm3903cHwGetPowerMax();
 
    *o = power_max;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetPowerMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float power_min;
    float *o = (float *)*out;

    power_min = itm3903cHwGetPowerMin();

    *o = power_min;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetPowerMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float power_max;

    uint32_t *p = (uint32_t *)in;

    power_max = *( (float *)p );

    itm3903cHwSetPowerMax(power_max);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetPowerMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float power_min;

    uint32_t *p = (uint32_t *)in;

    power_min = *( (float *)p );

    itm3903cHwSetPowerMin(power_min);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float current_max;
    float *o = (float *)*out;

    current_max = itm3903cHwGetCurrentMax();
 
    *o = current_max;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float current_min;
    float *o = (float *)*out;

    current_min = itm3903cHwGetCurrentMin();

    *o = current_min;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetCurrentMax(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float current_max;

    uint32_t *p = (uint32_t *)in;

    current_max = *( (float *)p );

    itm3903cHwSetCurrentMax(current_max);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetCurrentMin(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float current_min;

    uint32_t *p = (uint32_t *)in;

    current_min = *( (float *)p );

    itm3903cHwSetCurrentMin(current_min);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t volt_prot_status = itm3903cHwGetVoltageProtectionStatus();

    *o = volt_prot_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageUnderProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t volt_uprot_status = itm3903cHwGetVoltageUnderProtectionStatus();

    *o = volt_uprot_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltageProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setVoltProtStatus = *p;
    
    itm3903cHwSetVoltageProtectionStatus(setVoltProtStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    
    float *o = (float *) *out;

    float volt_oprot_value = itm3903cHwGetVoltageOverProtectionValue();

    *o = volt_oprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float *o = (float *) *out;

    float volt_uprot_value = itm3903cHwGetVoltageUnderProtectionValue();

    *o = volt_uprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetPowerProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t power_prot_status = itm3903cHwGetPowerProtectionStatus();

    *o = power_prot_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetPowerProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setPowerProtStatus = *p;
    
    itm3903cHwSetPowerProtectionStatus(setPowerProtStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetPowerOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float *o = (float *) *out;

    float pow_oprot_value = itm3903cHwGetPowerOverProtectionValue();

    *o = pow_oprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetPowerUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float *o = (float *) *out;

    float pow_uprot_value = itm3903cHwGetPowerUnderProtectionValue();

    *o = pow_uprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t curr_prot_status = itm3903cHwGetCurrentProtectionStatus();

    *o = curr_prot_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentUnderProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *o = (uint32_t *) *out;

    uint32_t curr_uprot_status = itm3903cHwGetCurrentUnderProtectionStatus();

    *o = curr_uprot_status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetCurrentProtectionStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    uint32_t *p = (uint32_t *)in;
    uint32_t setCurrProtStatus = *p;
    
    itm3903cHwSetCurrentProtectionStatus(setCurrProtStatus);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentOverProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float *o = (float *) *out;

    float curr_oprot_value = itm3903cHwGetCurrentOverProtectionValue();

    *o = curr_oprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetCurrentUnderProtection(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float *o = (float *) *out;

    float curr_uprot_value = itm3903cHwGetCurrentUnderProtectionValue();

    *o = curr_uprot_value;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetVoltageSlewRate(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float slew_rate;
    float *o = (float *)*out;

    slew_rate = itm3903cHwGetVoltageSlewRate();
 
    *o = slew_rate;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetVoltageSlewRate(void *in, uint32_t insize, void **out, uint32_t maxoutsize){
    float slew_rate;

    uint32_t *p = (uint32_t *)in;

    slew_rate = *( (float *)p );

    itm3903cHwSetVoltageSlewRate(slew_rate);
   
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetSamplingStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t status;

    status = *( (uint32_t *)in );

    itm3903cHwSetSamplingStatus(status);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetSamplingStatus(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *) *out;

    uint32_t status;

    status = itm3903cHwGetSamplingStatus();

    *o = status;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetSamplingFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t freq;

    freq = *( (uint32_t *)in );

    itm3903cHwSetSamplingFreq(freq);
    
    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetSamplingFreq(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    uint32_t *o = (uint32_t *) *out;

    uint32_t freq = itm3903cHwGetSamplingFreq();

    *o = freq;

    return 4;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetDacA1Offset(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float offset;

    offset = *( (float *)in );

    itm3903cHwDac1WriteOffset(offset);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetDacA1Adj(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float adj;

    adj = *( (float *)in );

    itm3903cHwDac1WriteAdj(adj);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetDacA2(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float value;

    value = *( (float *)in );

    itm3903cHwDac23WriteA2(value);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetDacA3(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float value;

    value = *( (float *)in );

    itm3903cHwDac23WriteA3(value);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetDacCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) in;

    itm3903cHwSetDacCalData(data);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetDacCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) *out;
    uint32_t calsize;

    calsize = itm3903cHwGetDacCalData(data);

    return calsize;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetAdcCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) in;

    itm3903cHwSetAdcCalData(data);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetAdcCalData(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) *out;
    uint32_t calsize;

    calsize = itm3903cHwGetAdcCalData(data);

    return calsize;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfSetAlphaValues(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) in;

    itm3903cHwSetAlphaValues(data);

    return 0;
}
//-----------------------------------------------------------------------------
static int32_t itm3903cHwIfGetAlphaValues(void *in, uint32_t insize, void **out, uint32_t maxoutsize){

    float *data = (float *) *out;
    uint32_t calsize;

    calsize = itm3903cHwGetAlphaValues(data);

    return calsize;
}
//-----------------------------------------------------------------------------
//=============================================================================

