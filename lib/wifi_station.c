#include "wifi_station.h"

#include <osapi.h>
#include <ets_sys.h>
#include <user_interface.h>
#include <espmissingincludes.h>

static const uint32_t IP_POLL_INTERVAL_MS = 100;

static os_timer_t ip_check_timer;
static uint32_t ip_time_counter_ms = 0;
static uint32_t ip_timeout_ms = 0;
static station_connect_cb connect_cb;

static void ip_check_cb(void *args) {
    os_timer_disarm(&ip_check_timer);
    ip_time_counter_ms += IP_POLL_INTERVAL_MS;

    uint8_t connect_status = wifi_station_get_connect_status();
    struct ip_info ipconfig;
    wifi_get_ip_info(STATION_IF, &ipconfig);

    if (connect_status == STATION_GOT_IP && ipconfig.ip.addr != 0) {
        // Everything's fine, tell the user that the connection succeeded
        connect_cb(true);
    } else if (connect_status == STATION_WRONG_PASSWORD ||
        connect_status == STATION_NO_AP_FOUND ||
        connect_status == STATION_CONNECT_FAIL ||
        ip_time_counter_ms > ip_timeout_ms) {
        connect_cb(false);
    } else {
        os_timer_arm(&ip_check_timer, IP_POLL_INTERVAL_MS, false);
    }
}

void wifi_station_init(const char *ssid, const char *password,
    station_connect_cb user_connect_cb, uint32_t timeout_ms) {
    wifi_set_opmode(STATION_MODE);
    struct station_config conf;
    conf.bssid_set = 0;
    os_memcpy(&conf.ssid, ssid, os_strlen(ssid) + 1);
    os_memcpy(&conf.password, password, os_strlen(password) + 1);
    wifi_station_set_config(&conf);

    ip_time_counter_ms = 0;
    ip_timeout_ms = timeout_ms;
    connect_cb = user_connect_cb;
    os_timer_disarm(&ip_check_timer);
    os_timer_setfn(&ip_check_timer, (os_timer_func_t *)ip_check_cb, NULL);
    os_timer_arm(&ip_check_timer, IP_POLL_INTERVAL_MS, false);
}

