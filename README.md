# Compatibility
Currently this firmware supports only KNOMI V1.

# Installation

1. Please include "knomi.cfg" from repo to your klipper installation.
You can comment out some sections if you don't have bed mesh or QGL.
2. Download latest release from GitHub releases - it will guide you on first installation. You'll have to connect your KNOMI display directly to PC.

# Upgrade
You can download filesystem / firmware from github releases and install them from KNOMI webpage.
If major version changed (1.0.0 -> 2.0.0) - then you MUST update filesystem (theme), it had some incompatible changes.
If minor version changed (1.0.0 -> 1.1.0) - then you can just upgrade firmware, all changes were backwards compatible.

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
