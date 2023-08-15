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
"""

for gif in gifs:
    parts = splitext(gif)
    print("Converting data/%s.gif to src/generated/%s.c" % (parts[0], parts[0]))
    filepath = "data/%s%s" % (parts[0],parts[1])
    filename = parts[0]
    out_path = "src/generated/%s.c" % parts[0]

    if (parts[1] == ".gif"): 
        conv = Converter(filepath, filename, False, Converter.FLAG.CF_RAW_CHROMA)
        conv.convert(Converter.FLAG.CF_RAW_CHROMA, 0)
    else:
        conv = Converter(filepath, filename, False, Converter.FLAG.CF_TRUE_COLOR_565)
        conv.convert(Converter.FLAG.CF_TRUE_COLOR_565, 0)

        cWrapper += rf"""
LV_IMG_DECLARE({parts[0]});

ResourceImage* get_{parts[0]}() {{
    return new ResourceImage(&{parts[0]});
}}
"""
        hWrapper += rf"""
ResourceImage* get_{parts[0]}();
"""
    
    c_arr = conv.format_to_c_array()

    with open(out_path, "w") as fi:
        res = conv.get_c_code_file(-1, c_arr)
        fi.write(res)

print("Generating images wrapper src/generated/images.h")
with open("src/generated/images.h", "w") as fi:
    fi.write(hWrapper)

print("Generating images wrapper src/generated/images.cpp")
with open("src/generated/images.cpp", "w") as fi:
    fi.write(cWrapper)
