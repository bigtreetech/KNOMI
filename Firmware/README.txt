To create suitable BMP files:

 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp


 Differences from original KNOMI Firmware:
 1) Firmware 2 times smaller, enabled OTA (in web browser knomi/update)
 2) All images are moved to second partition (theoretical possibility for easier theming + was required for OTA)
 3) Code tiny bit cleaner, cut all factory-test stuff.

 TODO:
 Cleanup code!
 Make OTA display progress on screen.
 Make BMP loading from fs a lot faster (on start)