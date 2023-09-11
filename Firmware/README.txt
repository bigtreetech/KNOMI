 Differences from original KNOMI Firmware:

 Pros:
 1) Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 2) All images are moved to second partition (theoretical possibility for easier theming + was required for OTA)
 3) Code a lot cleaner, cut all factory-test stuff, switched back to official esp32-arduino platform
 4) Nicer webui
 5) All communications are async - performance is better

 Cons:
 1) Startup logo loads slowly. First screen config (with Access Point instructions) is also slow to load.
 2) Unofficial fork - support is on best-effort basis

Development notes:

To create suitable BMP files:
 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp

For building you need nodejs installed and available on path.


TODO:
  - Brush up web interface to allow download/upload gifs and bmps from web interface directly to FS. Checks must be on frontend.
  - Hardcode version to frontend same way as in backend (if frontend to be moved to LittleFS)
    - Move frontend to LittleFS
  - Handle safe firmware check / boot to previous version using OTA
  - Make BMP loading from fs a lot faster (on start)  (e.g. try to make them an binary format instead of just bmp on spiffs) - especially noticeable on first launch / AP Config
  - Handle updates from github (both firmware and spiffs contents)
  - Add an option to select wifi from found networks.
  - To investigate:
      - Change first experience of wifi setup with smth better - e.g. WiFiManager lib
      - More universal captive portal - https://github.com/CDFER/Captive-Portal-ESP32/blob/main/src/main.cpp

