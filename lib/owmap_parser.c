#include "owmap_parser.h"

#include <espmissingincludes.h>

#include "images/icons_32.h"

static void start_arr(void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;

    if (os_strcmp(parser->current_key, "list") == 0) {
        parser->in_list = true;
        parser->array_level = 0;
        parser->object_level = 0;
    }

    parser->array_level += 1;
}

static void end_arr(void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;
    if (parser->in_list && (--(parser->array_level)) == 0) {
        parser->in_list = false;
    }
}

static void start_obj(void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;
    if (!parser->in_list) return;

    if (parser->object_level == 0) {
        parser->dt = 0;
        parser->temp = 0.0;
        parser->icon = ICON_NONE;
    }

    parser->object_level += 1;
}

static void end_obj(void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;
    if (!parser->in_list) return;

    weather_t *forecasts = parser->forecasts;
    int forecast_count = parser->forecast_count;
    if ((--(parser->object_level)) == 0 &&
        forecast_count < FORECAST_MAX_COUNT) {
        forecasts[forecast_count].time = parser->dt;
        forecasts[forecast_count].temp = parser->temp;
        forecasts[forecast_count].icon = parser->icon;
        parser->forecast_count += 1;
    }
}

static void obj_key(const char *key, size_t key_len, void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;
    os_strcpy(parser->current_key, key);
}

static void str(const char *value, size_t len, void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;
    if (os_strcmp(parser->current_key, "icon") == 0 &&
        parser->icon == ICON_NONE) {
        parser->icon = atoi(value);
    }
}

static void primitive(const char *value, size_t len, void *user_arg) {
    weather_parser_t *parser = (weather_parser_t *)user_arg;

    if (os_strcmp(parser->current_key, "dt") == 0 && parser->dt == 0) {
        parser->dt = atoi(value);
    } else if (os_strcmp(parser->current_key, "temp") == 0 &&
        parser->temp == 0) {
        parser->temp = atoi(value);
        char *point = os_strchr(value, '.');
        if (point != NULL && point - value > len && *(point + 1) >= '5') {
            if (value[0] == '-') parser->temp -= 1;
            else parser->temp += 1;
        }
    }
}

static jsmn_stream_callbacks_t cbs = {
    start_arr,
    end_arr,
    start_obj,
    end_obj,
    obj_key,
    str,
    primitive
};

void weather_parser_init(weather_parser_t *parser) {
    parser->current_key[0] = '\0';
    parser->array_level = 0;
    parser->object_level = 0;
    parser->in_list = false;
    parser->forecast_count = 0;
    jsmn_stream_init(&parser->json_parser, &cbs, parser);
}

void weather_stream_parse(weather_parser_t *parser, char c) {
    jsmn_stream_parse(&parser->json_parser, c);
}

unsigned char *get_weather_icon_bitmap(weather_icon_t icon) {
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

