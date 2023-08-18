from os.path import isfile, join, splitext
from os import makedirs, listdir, remove

Import("env")

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
    filename = parts[0]
    extension = parts[1]
    cWrapper += rf"""
ResourceImage* KnownResourceImages::get_{filename}(lv_coord_t x, lv_coord_t y) {{
    return new ResourceImage("{filename}{extension}", x, y);
}}
"""
    hWrapper += rf"""
    static ResourceImage* get_{filename}(lv_coord_t x = 0, lv_coord_t y = 0);
"""
    
hWrapper += """};"""

print("Generating images wrapper src/generated/images.h")
with open("src/generated/images.h", "w") as fi:
    fi.write(hWrapper)

print("Generating images wrapper src/generated/images.cpp")
with open("src/generated/images.cpp", "w") as fi:
    fi.write(cWrapper)