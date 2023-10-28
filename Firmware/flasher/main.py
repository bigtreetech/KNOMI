import os

import PyInstaller.__main__
import esptool

jsons = os.path.join(os.path.dirname(esptool.__file__), "targets", "stub_flasher")


PyInstaller.__main__.run([
    '--onefile',
    '--specpath', '../.build/',
    '--workpath', '../.build/work',
    '--distpath', '../.build/dist',
    '--add-binary', '../.pio/build/esp32dev/firmware_full.bin:.',
    '--add-binary', '../.pio/build/esp32dev/buildinfo.json:.',
    '--add-data', jsons + ':esptool/targets/stub_flasher',
    'flash_firmware.py'
])
