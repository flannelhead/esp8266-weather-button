//////////////////////////////////////////////////
// Simple NTP client for ESP8266.
// Copyright 2015 Richard A Burton
// richardaburton@gmail.com
// See license.txt for license terms.
//////////////////////////////////////////////////

#include <c_types.h>
#include <user_interface.h>
#include <espconn.h>
#include <osapi.h>
#include <mem.h>
#include <time.h>
#include <driver/uart.h>
#include <espmissingincludes.h>

#include "ntp.h"

static void ICACHE_FLASH_ATTR ntp_udp_recv(void *arg, char *pdata, unsigned short len) {
	struct espconn *pCon = (struct espconn *)arg;
	struct tm *dt;
	time_t timestamp;
	ntp_t *ntp;

	// extract ntp time
	ntp = (ntp_t*)pdata;
	timestamp = ntp->trans_time[0] << 24 | ntp->trans_time[1] << 16 |ntp->trans_time[2] << 8 | ntp->trans_time[3];
	// convert to unix time
	timestamp -= 2208988800UL;
	// create tm struct
	dt = gmtime(&timestamp);

	os_printf("timestamp: %ld\n", timestamp);

	if (pCon->reverse != NULL) {
		void (*ntp_callback)(time_t, struct tm *) = pCon->reverse;
		ntp_callback(timestamp, dt);
	}

	// clean up connection
	if (pCon) {
		espconn_delete(pCon);
		os_free(pCon->proto.udp);
		os_free(pCon);
		pCon = 0;
	}
}

void ICACHE_FLASH_ATTR ntp_get_time(uint8_t *ntp_server,
	void (*ntp_callback)(time_t timestamp, struct tm *dt)) {
	ntp_t ntp;

	// set up the udp "connection"
	struct espconn *pCon = (struct espconn*)os_zalloc(sizeof(struct espconn));
	pCon->type = ESPCONN_UDP;
	pCon->state = ESPCONN_NONE;
	pCon->proto.udp = (esp_udp*)os_zalloc(sizeof(esp_udp));
	pCon->proto.udp->local_port = espconn_port();
	pCon->proto.udp->remote_port = 123;
	pCon->reverse = ntp_callback;
	os_memcpy(pCon->proto.udp->remote_ip, ntp_server, 4);

	// create a really simple ntp request packet
	os_memset(&ntp, 0, sizeof(ntp_t));
	ntp.options = 0b00100011; // leap = 0, version = 4, mode = 3 (client)

	// send the ntp request
	espconn_create(pCon);
	espconn_regist_recvcb(pCon, ntp_udp_recv);
	espconn_send(pCon, (uint8*)&ntp, sizeof(ntp_t));
}

