/*
ESP8266 weather display firmware
Copyright (C) 2017  Sakari Kapanen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <driver/uart.h>

#include "u8g2.h"
#include "u8g2_esp8266_hal.h"

#include "credentials.h"

u8g2_t u8g2; // a structure which will contain all the data for one display

void oled_init(void) {
    u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_sw_i2c,
        u8x8_gpio_and_delay_esp8266);  // init u8g2 structure
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
}

void wifi_init(void) {
    wifi_set_opmode(STATION_MODE);
    struct station_config conf;
    conf.bssid_set = 0;
    os_memcpy(&conf.ssid, WIFI_SSID, os_strlen(WIFI_SSID) + 1);
    os_memcpy(&conf.password, WIFI_PWD, os_strlen(WIFI_PWD) + 1);
    wifi_station_set_config(&conf);
}

void user_init(void) {
    system_update_cpu_freq(80);
    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    wifi_init();

    os_printf("Hello World!\n");
}

