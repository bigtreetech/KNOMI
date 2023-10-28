import os

import PyInstaller.__main__
import esptool
import platform

jsons = os.path.join(os.path.dirname(esptool.__file__), "targets", "stub_flasher")


PyInstaller.__main__.run([
    '--onefile',
    '--noupx',
    '--specpath', '../.build/',
    '--workpath', '../.build/work',
    '--distpath', '../.build/dist',
    '--add-binary', '../.pio/build/esp32dev/firmware_full.bin:.',
    '--add-binary', '../.pio/build/esp32dev/buildinfo.json:.',
    '--add-data', jsons + ':esptool/targets/stub_flasher',
    '-n', 'flash_firmware.' + platform.system(),
    'flash_firmware.py'
])
