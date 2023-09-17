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
