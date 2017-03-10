/*
ESP8266 weather display firmware
Copyright (C) 2017 Sakari Kapanen

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
#include <espmissingincludes.h>
#include <driver/uart.h>

#include "u8g2.h"
#include "u8g2_esp8266_hal.h"

#include "httpclient.h"

#include "wifi_station.h"

#include "credentials.h"

#define CONNECTION_TIMEOUT 10000
#define DATA_FETCH_TIMEOUT 10000

os_timer_t timeout_timer;

u8g2_t u8g2;

void oled_init(void) {
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, U8G2_R2,
        u8x8_byte_esp8266_hw_spi,
        u8x8_gpio_and_delay_esp8266);  // init u8g2 structure
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g2, 0); // wake up display

    u8g2_ClearBuffer(&u8g2);
    u8g2_SetFontPosTop(&u8g2);
    u8g2_SetFont(&u8g2, u8g2_font_profont12_tf);
    u8g2_DrawStr(&u8g2, 0, 0, "Hello World!");
    u8g2_SendBuffer(&u8g2);
}

void http_get_callback(char * response_body, int http_status,
    char * response_headers, int body_size) {
    os_printf("HTTP status %d\n", http_status);

    if (response_headers != NULL) {
        os_printf("Headers:\n%s\n", response_headers);
    }

    if (response_body != NULL) {
        os_printf("Body:\n%s\n", response_body);
    }
}

void go_to_sleep(void) {
    os_printf("Going to sleep\n");
}

void wifi_connect_cb(bool connected) {
    if (connected) {
        os_printf("Connection succeeded\n");

        os_timer_disarm(&timeout_timer);
        os_timer_setfn(&timeout_timer, (os_timer_func_t *)go_to_sleep, NULL);
        os_timer_arm(&timeout_timer, DATA_FETCH_TIMEOUT, false);

        http_get_streaming("http://example.com", "", http_get_callback);  // Example domain for testing for now - this sends chunked responses
    } else {
        os_printf("Connection failed\n");
        go_to_sleep();
    }
}

void user_init(void) {
    system_update_cpu_freq(80);
    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    oled_init();

    wifi_station_init(WIFI_SSID, WIFI_PWD, wifi_connect_cb, CONNECTION_TIMEOUT);
}

