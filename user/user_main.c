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

#include "credentials.h"

void ICACHE_FLASH_ATTR wifi_init(void) {
    wifi_set_opmode(STATION_MODE);
    struct station_config conf;
    conf.bssid_set = 0;
    os_memcpy(&conf.ssid, WIFI_SSID, os_strlen(WIFI_SSID) + 1);
    os_memcpy(&conf.password, WIFI_PWD, os_strlen(WIFI_PWD) + 1);
    wifi_station_set_config(&conf);
}

void ICACHE_FLASH_ATTR user_init(void) {
    system_update_cpu_freq(80);
    uart_init(BIT_RATE_115200, BIT_RATE_115200);

    wifi_init();

    os_printf("Hello World!\n");
}

