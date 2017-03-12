#pragma once

#include <user_interface.h>

typedef enum {
    ICON_NONE = 0,
    CLEAR_SKY = 1,
    FEW_CLOUDS = 2,
    SCATTERED_CLOUDS = 3,
    BROKEN_CLOUDS = 4,
    SHOWER_RAIN = 9,
    RAIN = 10,
    THUNDERSTORM = 11,
    SNOW = 13,
    MIST = 50
} weather_icon_t;

static unsigned char clear_sky_bits[] ICACHE_RODATA_ATTR;
static unsigned char few_clouds_bits[] ICACHE_RODATA_ATTR;
static unsigned char scattered_clouds_bits[] ICACHE_RODATA_ATTR;
static unsigned char broken_clouds_bits[] ICACHE_RODATA_ATTR;
static unsigned char shower_rain_bits[] ICACHE_RODATA_ATTR;
static unsigned char rain_bits[] ICACHE_RODATA_ATTR;
static unsigned char thunderstorm_bits[] ICACHE_RODATA_ATTR;
static unsigned char snow_bits[] ICACHE_RODATA_ATTR;
static unsigned char mist_bits[] ICACHE_RODATA_ATTR;

unsigned char *get_icon_bitmap(weather_icon_t icon) {
    switch (icon) {
        case CLEAR_SKY:
            return clear_sky_bits;
        case FEW_CLOUDS:
            return few_clouds_bits;
        case SCATTERED_CLOUDS:
            return scattered_clouds_bits;
        case BROKEN_CLOUDS:
            return broken_clouds_bits;
        case SHOWER_RAIN:
            return shower_rain_bits;
        case RAIN:
            return rain_bits;
        case THUNDERSTORM:
            return thunderstorm_bits;
        case SNOW:
            return snow_bits;
        case MIST:
            return mist_bits;
        default:
            return NULL;
    }
}

#include "clear_sky.xbm"
#include "few_clouds.xbm"
#include "scattered_clouds.xbm"
#include "broken_clouds.xbm"
#include "shower_rain.xbm"
#include "rain.xbm"
#include "thunderstorm.xbm"
#include "snow.xbm"
#include "mist.xbm"

