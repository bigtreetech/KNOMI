from os import listdir, makedirs
from os.path import isfile, join, splitext
from os import listdir, remove
from pathlib import Path

Import("env")

try:
    from lv_img_converter import Converter

except ImportError:
    env.Execute("\"$PYTHONEXE\" -m pip install Pillow~=9.3.0")
    from lv_img_converter import Converter

gifs = [f for f in listdir('data') if isfile(join('data', f))]
gifs.sort()

makedirs(join("src", "generated"), exist_ok=True)
files = listdir(join("src", "generated"))
for file in files:
    file_path = join("src", "generated", file)
    if isfile(file_path):
        remove(file_path)

cWrapper = rf"""
#include "generated/images.h"
#include <Arduino.h>
#include <lvgl.h>

"""
hWrapper = rf"""
#pragma once

#include <lvgl.h>
#include "ResourceImage.h"

class KnownResourceImages {{
public:
"""

for gif in gifs:
    parts = splitext(gif)
    cWrapper += rf"""
LV_IMG_DECLARE({parts[0]})
"""

for gif in gifs:
    parts = splitext(gif)
    filename = parts[0]
    extension = parts[1]
    print("Converting data/%s.gif to src/generated/%s.c" % (filename, filename))
    filepath = "data/%s%s" % (filename,extension)
    out_path = "src/generated/%s.c" % filename
    isGif = (extension == ".gif")
    if isGif:
        conv = Converter(filepath, filename, False, Converter.FLAG.CF_RAW_CHROMA)
        conv.convert(Converter.FLAG.CF_RAW_CHROMA, 0)
    else:
        conv = Converter(filepath, filename, False, Converter.FLAG.CF_TRUE_COLOR_565)
        conv.convert(Converter.FLAG.CF_TRUE_COLOR_565, 0)


    cWrapper += rf"""
ResourceImage* KnownResourceImages::get_{filename}(lv_coord_t x, lv_coord_t y) {{
    return new ResourceImage(&{filename}, "{filename}{extension}", x, y);
}}
"""
    hWrapper += rf"""
    static ResourceImage* get_{filename}(lv_coord_t x = 0, lv_coord_t y = 0);
"""
    
    c_arr = conv.format_to_c_array()

    with open(out_path, "w") as fi:
        res = conv.get_c_code_file(-1, c_arr)
        fi.write(res)

hWrapper += """};"""

print("Generating images wrapper src/generated/images.h")
with open("src/generated/images.h", "w") as fi:
    fi.write(hWrapper)

print("Generating images wrapper src/generated/images.cpp")
with open("src/generated/images.cpp", "w") as fi:
    fi.write(cWrapper)
