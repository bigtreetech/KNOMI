# KNOMI
Online manual & customize UI tutorials：[here](https://bigtreetech.github.io/docs/KNOMI.html#)

# Differences from original KNOMI Firmware

## Pros:

 - Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 - All images are moved to second partition (theoretical possibility for easier theming + was required for OTA)
 - Code a lot cleaner, cut all factory-test stuff, switched back to official esp32-arduino platform
 - Nicer webui
 - All communications are async - performance is better

## Cons:

 - Startup logo loads slowly. First screen config (with Access Point instructions) is also slow to load.
 - First setup on iOS is not that nice. Enter "http://KNOMI/" to open initial setup in Safari. On Android / Windows regular captivity portal should open.
 - Unofficial fork - support is on best-effort basis

## Development notes:

To create suitable BMP files:
 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp

For building you need nodejs installed and available on path.
