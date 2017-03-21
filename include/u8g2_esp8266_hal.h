#pragma once

#include <stdint.h>

#include "eagle_soc.h"

#include "u8g2.h"

// Overclock the I2C bus
#define U8G2_ESP8266_HAL_I2C_FAST_PLUS

// Use SPI?
//#define U8G2_ESP8266_4WIRE_SPI


#ifdef U8G2_ESP8266_4WIRE_SPI
// This pin definition (SCK) applies only for the software SPI but the default
// is the same as the HW SCK pin.
#define U8G2_ESP8266_D0_MUX PERIPHS_IO_MUX_MTMS_U
#define U8G2_ESP8266_D0_FUNC FUNC_GPIO14
#define U8G2_ESP8266_D0_GPIO 14
// MOSI pin used by SW SPI. By default the same as the HW MOSI pin.
#define U8G2_ESP8266_D1_MUX PERIPHS_IO_MUX_MTCK_U
#define U8G2_ESP8266_D1_FUNC FUNC_GPIO13
#define U8G2_ESP8266_D1_GPIO 13

#define U8G2_ESP8266_RESET_MUX PERIPHS_IO_MUX_GPIO4_U
#define U8G2_ESP8266_RESET_FUNC FUNC_GPIO4
#define U8G2_ESP8266_RESET_GPIO 4

#define U8G2_ESP8266_DC_MUX PERIPHS_IO_MUX_GPIO5_U
#define U8G2_ESP8266_DC_FUNC FUNC_GPIO5
#define U8G2_ESP8266_DC_GPIO 5

#define U8G2_ESP8266_CS_MUX PERIPHS_IO_MUX_GPIO0_U
#define U8G2_ESP8266_CS_FUNC FUNC_GPIO0
#define U8G2_ESP8266_CS_GPIO 0

#else
// I2C pins: D0 == SCL, D1 == SDA. The implementation is software, use any pins
// you desire.
#define U8G2_ESP8266_D0_MUX PERIPHS_IO_MUX_GPIO5_U
#define U8G2_ESP8266_D0_FUNC FUNC_GPIO5
#define U8G2_ESP8266_D0_GPIO 5

#define U8G2_ESP8266_D1_MUX PERIPHS_IO_MUX_GPIO4_U
#define U8G2_ESP8266_D1_FUNC FUNC_GPIO4
#define U8G2_ESP8266_D1_GPIO 4
#endif

uint8_t u8x8_gpio_and_delay_esp8266(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_byte_esp8266_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
// Brzo_i2c by pasko-zh for a faster I2C implementation:
// https://github.com/pasko-zh/brzo_i2c
uint8_t u8x8_byte_brzo_sw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

