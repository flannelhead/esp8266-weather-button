# ESP8266 weather button

A simple weather display that is based on ESP8266. It connects to a WiFi network, fetches weather data from a weather API, stores it in the RTC memory and goes to deep sleep. After a fixed interval it wakes up and fetches the data again. If the user presses the reset button, the 128x64 OLED display is lighted and the stored weather forecast is displayed.

## Weather data source

The weather data is pulled from the [OpenWeatherMap API](https://openweathermap.org/api), which is available under [CC-BY-SA 4.0](http://creativecommons.org/licenses/by-sa/4.0/). Terms of use are listed [here](https://openweathermap.org/terms).

## Libraries

The [u8g2](https://github.com/olikraus/u8g2) graphics library by olikraus is included in this repo and is licensed under the BSD 2-clause license. The library also contains fonts which are licensed under various licenses. See the respective [LICENSE file](u8g2/LICENSE) for details.

The makefile has been adapted from Spritetm's [esphttpd](https://github.com/Spritetm/esphttpd) repo. In addition, there is a custom linker script `eagle.app.v6.custom.ld` which is the `eagle.app.v6.ld` from the Espressif SDK with one change: `.text` and `.literal` sections are located in `irom0` section by default, so there is no need to prefix functions with `ICACHE_FLASH_ATTR`. This also allows using the u8g2 code as-is.

[jsmn-stream](https://github.com/flannelhead/jsmn-stream) is a fork/rewrite of [jsmn](https://github.com/zserge/jsmn) by Serge Zaitsev (MIT license). It is used for incremental JSON parsing.

[esphttpclient](https://github.com/Caerbannog/esphttpclient) is used for requesting weather data over HTTP.

The NTP client is from Richard Burton's [esp8266 repository](https://github.com/raburton/esp8266), licensed under the MIT license.

## License

The code is released under GPLv3. See the LICENSE file for details.

