from os.path import isfile, join, splitext
from os import makedirs, listdir, remove
import shutil

Import("env")

def generate_fonts(*arg, **kwargs):
    usedFont = "Montserrat-Regular.ttf"

    # You must have nodejs/npm installed
    env.Execute(f"npx lv_font_conv --no-compress --no-prefilter --bpp 2 --size 32 --font fonts/{usedFont} -r 0x20-0x7F,0xB0,0x2022 --format lvgl -o src/generated/font_32.c --force-fast-kern-format")
    env.Execute(f"npx lv_font_conv --no-compress --no-prefilter --bpp 2 --size 48 --font fonts/{usedFont} -r 0x20-0x7F,0xB0,0x2022 --format lvgl -o src/generated/font_48.c --force-fast-kern-format")
    with open("src/generated/font_32.c", 'w+') as FILE:
        FILE.write("// TODO fix")
    with open("src/generated/font_48.c", 'w+') as FILE:
        FILE.write("// TODO fix")

env.AddCustomTarget(
    "generate_fonts",
    None,
    generate_fonts,
    title="generate_fonts",
    description="Generate fonts")

propName = "custom_generateFonts"
default = env.GetProjectConfig().get("env", propName)

if not BUILD_TARGETS and env.GetProjectOption(propName, default) == "true":
    generate_fonts();