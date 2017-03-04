#pragma once

#include <stdint.h>

#include "eagle_soc.h"

#include "u8g2.h"

#define SSD1306_D0_MUX PERIPHS_IO_MUX_MTMS_U
#define SSD1306_D0_FUNC FUNC_GPIO14
#define SSD1306_D0_GPIO 14
#define SSD1306_D1_MUX PERIPHS_IO_MUX_MTCK_U
#define SSD1306_D1_FUNC FUNC_GPIO13
#define SSD1306_D1_GPIO 13
#define SSD1306_RESET_MUX PERIPHS_IO_MUX_GPIO4_U
#define SSD1306_RESET_FUNC FUNC_GPIO4
#define SSD1306_RESET_GPIO 4
#define SSD1306_DC_MUX PERIPHS_IO_MUX_GPIO5_U
#define SSD1306_DC_FUNC FUNC_GPIO5
#define SSD1306_DC_GPIO 5
#define SSD1306_CS_MUX PERIPHS_IO_MUX_GPIO0_U
#define SSD1306_CS_FUNC FUNC_GPIO0
#define SSD1306_CS_GPIO 0

uint8_t u8x8_gpio_and_delay_esp8266(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

uint8_t u8x8_byte_esp8266_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);

