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

#include "ntp.h"
#include "httpclient.h"
#include "wifi_station.h"
#include "owmap_parser.h"

#include "util.h"
#include "credentials.h"
#include "my_config.h"

// All below are milliseconds
#define CONNECTION_TIMEOUT 10000
#define DATA_FETCH_TIMEOUT 10000
#define SCREEN_TIMEOUT 20000
#define DATA_FETCH_INTERVAL (5*60000)

#define FORECAST_MAX_COUNT 8

#define MAGIC_NUM 0x55aaaa55

os_timer_t timeout_timer;

u8g2_t u8g2;
weather_parser_t wparser;
bool idle_fetch;

void sleep_timeout(uint32_t timeout);

void oled_draw_forecast(int x, int y, const weather_t *forecast,
    bool draw_weekday) {
    int dx;

    u8g2_SetFontPosTop(&u8g2);

    struct tm *dt = gmtime(&forecast->time);
    char buf[32];
    if (draw_weekday) {
        os_sprintf(buf, "%s %02d", WEEKDAYS[dt->tm_wday], dt->tm_hour);
    } else {
        os_sprintf(buf, "%02d", dt->tm_hour);
    }
    dx = (32 - u8g2_GetUTF8Width(&u8g2, buf)) / 2;
    u8g2_DrawUTF8(&u8g2, x + dx, y, buf);

    const uint8_t *bitmap = get_weather_icon_bitmap(forecast->icon);
    if (bitmap != NULL) {
        u8g2_DrawXBMP(&u8g2, x, y + 14, 32, 32, bitmap);
    }

    os_sprintf(buf, "%d°C", forecast->temp);
    dx = (32 - u8g2_GetUTF8Width(&u8g2, buf)) / 2;
    u8g2_DrawUTF8(&u8g2, x + dx, y + 52, buf);
}

void oled_draw_forecasts(const weather_t *forecasts, int n_forecasts) {
    u8g2_ClearBuffer(&u8g2);

    int j, c;
    weather_t my_forecasts[3];
    for (j = 0, c = 0; j < n_forecasts && c < 3; ++j) {
        struct tm *dt = gmtime(&forecasts[j].time);
        if (dt->tm_hour <= 18 && dt->tm_hour >= 9) {
            my_forecasts[c] = forecasts[j];
            c += 1;
        }
    }
    if (c < 3) return;

    int prev_wday = 7;
    for (int i = 0; i < 3; ++i) {
        struct tm *dt = gmtime(&my_forecasts[i].time);
        oled_draw_forecast(2 + i*46, 0, &my_forecasts[i], dt->tm_wday != prev_wday);
        prev_wday = dt->tm_wday;
    }

    u8g2_SendBuffer(&u8g2);
}

void oled_init(void) {
    u8g2_SetPowerSave(&u8g2, 0); // wake up display
    u8g2_SetFont(&u8g2, u8g2_font_profont12_tf);
    u8g2_ClearDisplay(&u8g2);
}

void forecast_display() {
    uint32_t n_forecasts = 0;
    if (system_rtc_mem_read(65, &n_forecasts, 4) && n_forecasts != 0) {
        os_printf("Found %u forecasts\n", n_forecasts);
        wparser.forecast_count = n_forecasts;
        system_rtc_mem_read(67, wparser.forecasts,
            n_forecasts * sizeof(weather_t));
    }
    oled_init();
    oled_draw_forecasts(wparser.forecasts, n_forecasts);
    sleep_timeout(SCREEN_TIMEOUT);
}

void go_to_sleep(uint32_t sleep_timeout) {
    os_printf("Going to sleep, timeout = %u\n", sleep_timeout);
    u8g2_SetPowerSave(&u8g2, 1); // put display to sleep
    system_deep_sleep(1000 * sleep_timeout);
}

void sleep_timer_cb(void *arg) {
    uint32_t *timeout = (uint32_t *)arg;
    go_to_sleep(*timeout);
}

void http_get_callback(char * response_body, int http_status,
    char * response_headers, int body_size) {
    static int current_status = 0;
    if (response_headers != NULL) {
        current_status = http_status;
        weather_parser_init(&wparser);
    }
    if (current_status == 200 && response_body != NULL) {
        char ch;
        while ((ch = *(response_body++)) != '\0') {
            weather_stream_parse(&wparser, ch);
        }
    }

    if (http_status == HTTP_STATUS_DISCONNECT) {
        os_timer_disarm(&timeout_timer);
        uint32_t data_length = wparser.forecast_count;
        uint32_t flag = 0;
        system_rtc_mem_write(64, &flag, 4);
        system_rtc_mem_write(65, &data_length, 4);
        system_rtc_mem_write(67, wparser.forecasts, data_length * sizeof(weather_t));
        flag = MAGIC_NUM;
        system_rtc_mem_write(64, &flag, 4);
        os_printf("Fetched %u forecasts\n", data_length);
        if (!idle_fetch) {
            forecast_display();
        } else {
            go_to_sleep(DATA_FETCH_INTERVAL);
        }
    }
}

char owmap_query[128];
void do_owmap_query(void) {
    os_sprintf(owmap_query,
        "http://api.openweathermap.org/data/2.5/forecast?id=%s&appid=%s&units=metric",
        OWMAP_CITY_ID, OWMAP_API_KEY);

    http_get(owmap_query, "", http_get_callback);  // Example domain for testing for now - this sends chunked responses
}

void ntp_cb(time_t timestamp, struct tm *dt) {
    apply_tz(dt, TIMEZONE_OFFSET);
    os_printf("Date %d.%d, time %d.%d\n",
        dt->tm_mday, dt->tm_mon + 1, dt->tm_hour, dt->tm_min);
    do_owmap_query();
}

void ntp_dns_cb(uint8_t *addr) {
    if (addr == NULL) {
        os_printf("Error resolving NTP server address\n");
        return;
    }

    ntp_get_time(addr, ntp_cb);
}

uint32_t data_fetch_interval = DATA_FETCH_INTERVAL;
void sleep_timeout(uint32_t timeout) {
    os_timer_disarm(&timeout_timer);
    os_timer_setfn(&timeout_timer, (os_timer_func_t *)sleep_timer_cb,
        &data_fetch_interval);
    os_timer_arm(&timeout_timer, timeout, false);
}

void wifi_connect_cb(bool connected) {
    if (connected) {
        sleep_timeout(DATA_FETCH_TIMEOUT);
        //dns_resolve("time.nist.gov", ntp_dns_cb);
        do_owmap_query();
    } else {
        go_to_sleep(DATA_FETCH_INTERVAL);
    }
}

void fetch_weather_data(void) {
    wifi_station_init(WIFI_SSID, WIFI_PWD, wifi_connect_cb, CONNECTION_TIMEOUT);
}

void user_init(void) {
    uint16_t adc = system_adc_read();
    system_update_cpu_freq(80);
    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0,
        u8x8_byte_brzo_sw_i2c,
        //u8x8_byte_sw_i2c,
        u8x8_gpio_and_delay_esp8266);  // init u8g2 structure
    u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this

    idle_fetch = false;
    if (adc > 850) {
        // We've woken up to update the data
        idle_fetch = true;
        os_printf("Doing idle fetch...\n");
        fetch_weather_data();
    } else {
        uint32_t flag = 0;
        if (system_rtc_mem_read(64, &flag, 4) && flag == MAGIC_NUM) {
            os_printf("Displaying data directly from RTC...\n");
            forecast_display();
        } else {
            os_printf("Going to display data, fetching first...\n");
            fetch_weather_data();
        }
    }
}

