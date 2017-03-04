# ESP8266 weather button

A simple weather display that is based on ESP8266. It connects to a WiFi network, fetches weather data from a weather API, stores it in the RTC memory and goes to deep sleep. After a fixed interval it wakes up and fetches the data again. If the user presses the reset button, the 128x64 OLED display is lighted and the stored weather forecast is displayed.

## Libraries

The [u8g2](https://github.com/olikraus/u8g2) graphics library by olikraus is included in this repo and is licensed under the BSD 2-clause license. The library also contains fonts which are licensed under various licenses. See the respective [LICENSE file](u8g2/LICENSE) for details.

## License

The code is released undex GPLv3. See the LICENSE file for details.

