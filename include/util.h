#pragma once

#include <time.h>

void dns_resolve(const char *hostname, void (*dns_callback)(uint8_t *ip));

void apply_tz(struct tm *time, int tz_offset);

