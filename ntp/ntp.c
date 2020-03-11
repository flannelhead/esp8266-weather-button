//////////////////////////////////////////////////
// Simple NTP client for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//
// Modified by Sakari Kapanen in 2017.
//////////////////////////////////////////////////

#include <c_types.h>
#include <user_interface.h>
#include <osapi.h>
#include <mem.h>
#include <time.h>
#include <driver/uart.h>
#include <espmissingincludes.h>
#include <lwip/udp.h>

#include "ntp.h"

static void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, struct udp_pcb *pcb,
	struct pbuf *pb, ip_addr_t *addr, uint16_t port) {
	void (*ntp_callback)(time_t timestamp, struct tm *dt) = arg;
	struct tm *dt;
	time_t timestamp;
	ntp_t *ntp;

	// extract ntp time
	ntp = (ntp_t*)pb->payload;
	timestamp = ntp->trans_time[0] << 24 | ntp->trans_time[1] << 16 |ntp->trans_time[2] << 8 | ntp->trans_time[3];
	// convert to unix time
	timestamp -= 2208988800UL;
	// create tm struct
	dt = gmtime(&timestamp);

	if (ntp_callback != NULL) {
		ntp_callback(timestamp, dt);
	}

	udp_remove(pcb);
}

void ICACHE_FLASH_ATTR ntp_get_time(uint8_t *ntp_server,
	void (*ntp_callback)(time_t timestamp, struct tm *dt)) {
	// set up the udp "connection"
	ip_addr_t addr;
	os_memcpy(&addr.addr, ntp_server, 4);
	struct pbuf * pb = pbuf_alloc(PBUF_TRANSPORT, sizeof(ntp_t), PBUF_RAM);
	ntp_t *ntp = (ntp_t *)pb->payload;
	// create a really simple ntp request packet
	os_memset(ntp, 0, sizeof(ntp_t));
	ntp->options = 0b00100011; // leap = 0, version = 4, mode = 3 (client)
	struct udp_pcb *pcb = udp_new();
	udp_recv(pcb, ntp_udp_recv, ntp_callback);
	udp_sendto(pcb, pb, &addr, 123);
	pbuf_free(pb);
}

