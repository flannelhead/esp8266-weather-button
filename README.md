# ESP8266 weather button

A simple weather display that is based on ESP8266. It connects to a WiFi network, fetches weather data from a weather API, stores it in the RTC memory and goes to deep sleep. After a fixed interval it wakes up and fetches the data again. If the user presses the reset button, the 128x64 OLED display is lighted and the stored weather forecast is displayed.

## Arduino implementation

The initial prototype implementation uses the Arduino ESP8266 SDK. It makes use of the [esp8266-weather-station](https://github.com/squix78/esp8266-weather-station) library by squix78.o

If I have enough time and interest, I might implement this from scratch using the ESP8266 NON-OS SDK for the sake of learning and having fun.

## License

The code is released undex GPLv3. See the LICENSE file for details.

