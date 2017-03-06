/*
A helper for connecting the ESP8266 to a WiFi access point and checking the
connection status
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

#pragma once

#include <stdint.h>
#include <c_types.h>

// The callback gets an argument: whether connection succeeded or not
typedef void (*station_connect_cb)(bool connected);

void wifi_station_init(const char *ssid, const char *password,
    station_connect_cb user_connect_cb, uint32_t timeout_ms);

