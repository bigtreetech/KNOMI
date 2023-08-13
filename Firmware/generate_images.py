from os import listdir, makedirs
from os.path import isfile, join, splitext
from pathlib import Path

Import("env")

try:
    from lv_img_converter import Converter

except ImportError:
    env.Execute("$PYTHONEXE -m pip install Pillow~=9.3.0")
    from lv_img_converter import Converter

def conv_one_file(filepath, filename, out_path):
    conv = Converter(filepath, filename, False, Converter.FLAG.CF_RAW_CHROMA)

    conv.convert(Converter.FLAG.CF_RAW_CHROMA, 0)
    c_arr = conv.format_to_c_array()

    with open(out_path, "w") as fi:
        res = conv.get_c_code_file(-1, c_arr)
        fi.write(res)
    return "SUCCESS"

gifs = [f for f in listdir('GIFS') if isfile(join('GIFS', f))]

makedirs(join("src", "generated"), exist_ok=True)

for gif in gifs:
    parts = splitext(gif)
    print("Converting: %s" % parts[0])
    conv_one_file("GIFS/%s%s" % (parts[0],parts[1]), parts[0], "src/generated/%s.c" % parts[0])