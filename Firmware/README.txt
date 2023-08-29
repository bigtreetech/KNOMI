To create suitable BMP files:

 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp


 Differences from original KNOMI Firmware:
 1) Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 2) All images are moved to second partition (theoretical possibility for easier theming + was required for OTA)
 3) Code a lot cleaner, cut all factory-test stuff.

 TODO:
  - Cleanup code!
  - Make OTA display progress on screen.
  - Make BMP loading from fs a lot faster (on start)  (e.g. try to make them an binary format instead of just bmp on spiffs) - especially noticeable on first launch / AP Config
  - Try to move lv_task_handler to native ESP32 vTask - so that delay in Arduino's HTTP Client won't freeze it.
  - Brush up web interface to allow download/upload gifs from web interface directly to FS.
  - Handle safe firmware check / boot to previous version using OTA
  - Change first experience of wifi setup with smth better - e.g. WiFiManager lib
  - Switch webserver to ESPAsyncWebserver
  - More universal captive portal - https://github.com/CDFER/Captive-Portal-ESP32/blob/main/src/main.cpp