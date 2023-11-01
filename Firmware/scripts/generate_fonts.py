from os.path import isfile, join, splitext
from os import makedirs, listdir, remove
import shutil

Import("env")

def generate_fonts(*arg, **kwargs):
    print("Use https://rop.nl/truetype2gfx/ instead. or to be ported at some type in future...")

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