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

#include "jsmn.h"

#include "httpclient.h"

#include "wifi_station.h"

#include "credentials.h"
#include "my_config.h"

#define CONNECTION_TIMEOUT 10000
#define DATA_FETCH_TIMEOUT 10000

#define SLEEP_INTERVAL 5000

os_timer_t timeout_timer;

u8g2_t u8g2;
jsmn_parser parser;

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

void start_arr(void) {
    os_printf("Array started\n");
}
void end_arr(void) {
    os_printf("Array ended\n");
}
void start_obj(void) {
    os_printf("Object started\n");
}
void end_obj(void) {
    os_printf("Object ended\n");
}
void obj_key(const char *key, size_t key_len) {
    os_printf("Object key: %s\n", key);
}
void str(const char *value, size_t len) {
    os_printf("String: %s\n", value);
}
void primitive(const char *value, size_t len) {
    os_printf("Primitive: %s\n", value);
}

jsmn_callbacks_t cbs = {
    start_arr,
    end_arr,
    start_obj,
    end_obj,
    obj_key,
    str,
    primitive
};

void go_to_sleep(void) {
    os_printf("Going to sleep\n");
    u8g2_SetPowerSave(&u8g2, 1); // put display to sleep
    system_deep_sleep(1000 * SLEEP_INTERVAL);
}

void http_get_callback(char * response_body, int http_status,
    char * response_headers, int body_size) {
    os_printf("HTTP status %d\n", http_status);

    if (response_headers != NULL) {
        os_printf("Headers:\n%s\n", response_headers);
    }

    if (response_body != NULL) {
        char ch;
        while ((ch = *(response_body++)) != '\0') {
            jsmn_parse(&parser, ch);
        }
    }

    if (http_status == HTTP_STATUS_DISCONNECT) {
        go_to_sleep();
    }
}

char owmap_query[128];
void wifi_connect_cb(bool connected) {
    if (connected) {
        os_printf("Connection succeeded\n");

        os_timer_disarm(&timeout_timer);
        os_timer_setfn(&timeout_timer, (os_timer_func_t *)go_to_sleep, NULL);
        os_timer_arm(&timeout_timer, DATA_FETCH_TIMEOUT, false);

        os_sprintf(owmap_query,
            "http://api.openweathermap.org/data/2.5/forecast?id=%s&appid=%s&units=metric",
            OWMAP_CITY_ID, OWMAP_API_KEY);

        http_get_streaming(owmap_query, "", http_get_callback);  // Example domain for testing for now - this sends chunked responses
    } else {
        os_printf("Connection failed\n");
        go_to_sleep();
    }
}

void user_init(void) {
    system_update_cpu_freq(80);
    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    oled_init();

    jsmn_init(&parser, &cbs);

    wifi_station_init(WIFI_SSID, WIFI_PWD, wifi_connect_cb, CONNECTION_TIMEOUT);
}

