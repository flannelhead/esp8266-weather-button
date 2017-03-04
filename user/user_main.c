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

#define QUEUE_LEN 1

os_event_t task_queue[QUEUE_LEN];
u8g2_t u8g2; // a structure which will contain all the data for one display

void compute(os_event_t *e) {
    static uint32_t time = 0;
    system_os_post(2, 0, 0);

    uint32_t new_time = system_get_time();
    os_printf("%lu\n", new_time - time);
    time = new_time;

    uint32_t x = new_time * 128 / 500000;

    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawCircle(&u8g2, x, 32, 10, U8G2_DRAW_ALL);
    u8g2_SendBuffer(&u8g2);
}

void oled_init(void) {
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R0,
        u8x8_byte_esp8266_hw_spi,
        u8x8_gpio_and_delay_esp8266);  // init u8g2 structure
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    u8g2_SetContrast(&u8g2, 255);

    u8g2_ClearBuffer(&u8g2);
    u8g2_DrawCircle(&u8g2, 10, 10, 10, U8G2_DRAW_ALL);
    u8g2_SendBuffer(&u8g2);
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

    oled_init();

    os_printf("Hello World!\n");

    system_os_task(compute, 2, task_queue, QUEUE_LEN);
    system_os_post(2, 0, 0);
}

