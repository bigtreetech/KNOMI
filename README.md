# KNOMI
Online manual & customize UI tutorials：[here](https://bigtreetech.github.io/docs/KNOMI.html#)

# Differences from original KNOMI Firmware

## Pros:

 - Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 - All images are moved to second partition 
 - Code a lot cleaner, cut all factory-test stuff, switched back to official esp32-arduino platform
 - Nicer webui
 - Can change images using webui, without reflashing firmware
 - Any size of gifs are supported, even 240x240
 - All communications are async - performance is better

## Cons:

 - First setup on iOS is not that nice. Enter "http://KNOMI/" to open initial setup in Safari. On Android / Windows regular captivity portal should open.
 - Unofficial fork - support is on best-effort basis

## Development notes:

For building you need nodejs installed and available on path.
