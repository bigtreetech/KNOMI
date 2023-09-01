To create suitable BMP files:

 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp

For building you need nodejs installed and available on path.

 Differences from original KNOMI Firmware:
 1) Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 2) All images are moved to second partition (theoretical possibility for easier theming + was required for OTA)
 3) Code a lot cleaner, cut all factory-test stuff.

 TODO:
  - Brush up web interface to allow download/upload gifs from web interface directly to FS.
  - Make OTA display progress on screen.
  - Hardcode version to frontend same way as in backend (if frontend to be moved to LittleFS)
  - Handle safe firmware check / boot to previous version using OTA
  - Merge ElegantOTA web interface into KNOMI one.
  - Make BMP loading from fs a lot faster (on start)  (e.g. try to make them an binary format instead of just bmp on spiffs) - especially noticeable on first launch / AP Config
  - Try to move lv_task_handler to native ESP32 vTask - so that delay in Arduino's HTTP Client won't freeze it.
  - Change first experience of wifi setup with smth better - e.g. WiFiManager lib
  - More universal captive portal - https://github.com/CDFER/Captive-Portal-ESP32/blob/main/src/main.cpp
  - Handle updates from github (both firmware and spiffs contents)
  - Add an option to select wifi from found networks.
