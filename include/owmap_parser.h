#pragma once

#include <time.h>
#include <user_interface.h>

#include "jsmn_stream.h"

#define FORECAST_MAX_COUNT 8

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

typedef struct {
    time_t time;
    int temp;
    weather_icon_t icon;
} weather_t;

typedef struct {
    char current_key[64];
    int array_level;
    int object_level;
    bool in_list;
    long dt;
    int temp;
    weather_icon_t icon;
    weather_t forecasts[FORECAST_MAX_COUNT];
    int forecast_count;
    jsmn_stream_parser json_parser;
} weather_parser_t;

void weather_parser_init(weather_parser_t *parser);
void weather_stream_parse(weather_parser_t *parser, char c);

unsigned char *get_weather_icon_bitmap(weather_icon_t icon);

