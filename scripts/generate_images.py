from os.path import isfile, join, splitext
from os import makedirs, listdir, remove

Import("env")

def generate_images(*arg, **kwargs):
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

    """
    hWrapper = rf"""
    #pragma once

    #include "ui/ResourceImage.h"
    #include <vector>
    #include "log.h"
    #include <Arduino.h>

    class KnownResourceImages {{
    public:
        static std::vector<const char*> enumerateFiles();
    """
        
    for gif in gifs:
        parts = splitext(gif)
        filename = parts[0]
        extension = parts[1]
        cWrapper += rf"""
    const PROGMEM char* FILE_{filename} = "{filename}{extension}";"""

    cWrapper += rf"""

    std::vector<const char*> KnownResourceImages::enumerateFiles() {{
        auto result = std::vector<const char*>();
"""

    for gif in gifs:
        parts = splitext(gif)
        filename = parts[0]
        cWrapper += rf"""
        result.push_back(FILE_{filename});"""

    cWrapper += r"""
        return result;
    }
    """

    for gif in gifs:
        parts = splitext(gif)
        filename = parts[0]
        extension = parts[1]
        cWrapper += rf"""
    ResourceImage* KnownResourceImages::get_{filename}(int x, int y) {{
        return new ResourceImage(FILE_{filename}, x, y);
    }}
    """
        hWrapper += rf"""
        static ResourceImage* get_{filename}(int x = 0, int y = 0);
    """
        
    hWrapper += """};"""

    print("Generating images wrapper src/generated/images.h")
    with open("src/generated/images.h", "w") as fi:
        fi.write(hWrapper)

    print("Generating images wrapper src/generated/images.cpp")
    with open("src/generated/images.cpp", "w") as fi:
        fi.write(cWrapper)


env.AddCustomTarget(
    "generate_images",
    None,
    generate_images,
    title="generate_images",
    description="Generate images")

propName = "custom_generateImages"
default = env.GetProjectConfig().get("env", propName)

if not BUILD_TARGETS and env.GetProjectOption(propName, default) == "true":
    generate_images();