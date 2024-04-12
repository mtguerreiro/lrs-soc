/*
 * itm3903cConfig.h
 *
 */

#ifndef ITM3903C_PICO_CONFIG_H_
#define ITM3903C_PICO_CONFIG_H_

//=============================================================================
/*-------------------------------- Includes ---------------------------------*/
//=============================================================================
#include "stdint.h"
//=============================================================================

//=============================================================================
/*------------------------------- Definitions -------------------------------*/
//=============================================================================

/* ADC conversion */
#define ITM3903C_PICO_CONFIG_ADC_GAIN_INV           (float)((3.3f / 4095.0f))

/* RS232-UART */
#define ITM3903C_PICO_CONFIG_RS232_UART             uart1
#define ITM3903C_PICO_CONFIG_RS232_UART_BAUD_RATE   9600
#define ITM3903C_PICO_CONFIG_RS232_UART_TX_PIN      4
#define ITM3903C_PICO_CONFIG_RS232_UART_RX_PIN      5
#define ITM3903C_PICO_CONFIG_RS232_UART_TO_MS       (100)

/* SPI for DACs */
#define ITM3903C_PICO_CONFIG_DAC_SPI                spi1
#define ITM3903C_PICO_CONFIG_DAC_SPI_CLK            (1 * 1000 * 1000) /* Hz */
#define ITM3903C_PICO_CONFIG_DAC_SPI_CLK_PIN        10
#define ITM3903C_PICO_CONFIG_DAC_SPI_SD_PIN         11
#define ITM3903C_PICO_CONFIG_DAC_A1_CS_PIN          12
#define ITM3903C_PICO_CONFIG_DAC_A2_A3_CS_PIN       13

/* Relays */
#define ITM3903C_PICO_RELAY_1                       14
#define ITM3903C_PICO_RELAY_2                       15

/* Additional LEDs */
#define ITM3903C_PICO_LED_1                         6
#define ITM3903C_PICO_LED_2                         7

/* PWM slice for timer */
#define ITM3903C_PICO_CONFIG_PWM_SLICE              0
#define ITM3903C_PICO_CONFIG_PWM_CLK_DIV            125
//=============================================================================

#endif /* ITM3903C_PICO_CONFIG_H_ */
