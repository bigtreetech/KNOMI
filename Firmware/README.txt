To create suitable BMP files:

 # ffmpeg -i source.gif -sws_flags neighbor -sws_dither none -pix_fmt rgb565 -y result.bmp