#pragma once

#include <stdint.h>
#include <c_types.h>

// The callback gets an argument: whether connection succeeded or not
typedef void (*station_connect_cb)(bool connected);

void wifi_station_init(const char *ssid, const char *password,
    station_connect_cb user_connect_cb, uint32_t timeout_ms);

