Import("env")
import requests

def parse_coredump(*arg, **kwargs):
    try:
        from esp_coredump import CoreDump
    except ImportError:
        env.Execute("\"$PYTHONEXE\" -m pip install esp-coredump~=1.7.0")
        from esp_coredump import CoreDump   

    url = 'http://knomi/api/coredump'
    r = requests.get(url, allow_redirects=True)
    open('./.pio/coredump', 'wb').write(r.content)

    coredump = CoreDump(chip='esp32',core="./.pio/coredump",core_format='raw', prog='./.pio/build/esp32dev/firmware.elf')
    coredump.info_corefile()  #  print the info of the test app corefile


env.AddCustomTarget(
    "parse_coredump",
    None,
    parse_coredump,
    title="parse_coredump",
    description="Query coredump and parse it")