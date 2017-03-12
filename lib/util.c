#include <user_interface.h>
#include <espconn.h>

#include "util.h"

static void ICACHE_FLASH_ATTR dns_found(const char *name, ip_addr_t *addr,
    void *arg) {
    void (*callback)(const uint8_t *) = arg;
    if (addr != NULL) {
        callback((uint8_t *)(&addr->addr));
    } else {
        callback(NULL);
    }
}

void ICACHE_FLASH_ATTR dns_resolve(const char *hostname,
    void (*dns_callback)(uint8_t *ip)) {
    ip_addr_t addr;
    err_t ret = espconn_gethostbyname((struct espconn *)dns_callback, hostname,
        &addr, dns_found);
    if (ret == ESPCONN_OK) {
        dns_callback((uint8_t *)(&addr.addr));
    } else if (ret != ESPCONN_INPROGRESS) {
        dns_callback(NULL);
    }
}

//////////////////////////////////////////////////
// Simple timezone example for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt in folder "ntp" for license terms.
//////////////////////////////////////////////////

// custom function to apply a timezone to the supplied tm struct
// hard coded rules
void ICACHE_FLASH_ATTR applyTZ(struct tm *time, int tz_offset) {
    bool dst = false;

    // apply base timezone offset
    //time->tm_hour += 1; // e.g. central europe

    // call mktime to fix up (needed if applying offset has rolled the date back/forward a day)
    // also sets yday and fixes wday (if it was wrong from the rtc)
    mktime(time);

    // work out if we should apply dst, modify according to your local rules
    if (time->tm_mon < 2 || time->tm_mon > 9) {
        // these months are completely out of DST
    } else if (time->tm_mon > 2 && time->tm_mon < 9) {
        // these months are completely in DST
        dst = true;
    } else {
        // else we must be in one of the change months
        // work out when the last sunday was (could be today)
        int previousSunday = time->tm_mday - time->tm_wday;
        if (time->tm_mon == 2) { // march
            // was last sunday (which could be today) the last sunday in march
            if (previousSunday >= 25) {
                // are we actually on the last sunday today
                if (time->tm_wday == 0) {
                    // if so are we at/past 2am gmt
                    int s = (time->tm_hour * 3600) + (time->tm_min * 60) + time->tm_sec;
                    if (s >= 7200) dst = true;
                } else {
                    dst = true;
                }
            }
        } else if (time->tm_mon == 9) {
            // was last sunday (which could be today) the last sunday in october
            if (previousSunday >= 25) {
                // we have reached/passed it, so is it today?
                if (time->tm_wday == 0) {
                    // change day, so are we before 1am gmt (2am localtime)
                    int s = (time->tm_hour * 3600) + (time->tm_min * 60) + time->tm_sec;
                    if (s < 3600) dst = true;
                }
            } else {
                // not reached the last sunday yet
                dst = true;
            }
        }
    }

    if (dst) {
        // add the dst hour
        time->tm_hour += 1;
        // mktime will fix up the time/date if adding an hour has taken us to the next day
        mktime(time);
        // don't rely on isdst returned by mktime, it doesn't know about timezones and tends to reset this to 0
        time->tm_isdst = 1;
    } else {
        time->tm_isdst = 0;
    }
}
