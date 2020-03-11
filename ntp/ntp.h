//////////////////////////////////////////////////
// Simple NTP client for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//
// Modified by Sakari Kapanen in 2017.
//////////////////////////////////////////////////

#ifndef __NTP_H__
#define __NTP_H__

#include <time.h>

typedef struct {
	uint8 options;
	uint8 stratum;
	uint8 poll;
	uint8 precision;
	uint32 root_delay;
	uint32 root_disp;
	uint32 ref_id;
	uint8 ref_time[8];
	uint8 orig_time[8];
	uint8 recv_time[8];
	uint8 trans_time[8];
} ntp_t;

void ICACHE_FLASH_ATTR ntp_get_time(uint8_t *ntp_server,
	void (*ntp_callback)(time_t timestamp, struct tm *dt));

#endif
