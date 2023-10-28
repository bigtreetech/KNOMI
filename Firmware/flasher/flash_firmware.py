import esptool
import os
import sys
import json

current_directory = __dir__ = os.path.dirname(os.path.abspath(__file__))

fn = os.path.join(current_directory, "buildinfo.json")

with open(fn, 'r') as json_file:
    buildInfo = json.loads(json_file.read())

# Display the disclaimer
disclaimer = f"""
KNOMI Firmware Installer 
==================================================================

Firmware info: https://github.com/DiverOfDark/KNOMI
Branch: {buildInfo["branch"]} ({buildInfo["commit"]})
Source Timestamp: {buildInfo["commitTime"]}
Binary Timestamp: {buildInfo["buildTime"]}

==================================================================
  
This tool will flash alternative KNOMI firmware to your device.
Please put your KNOMI into flash mode (press button and connect cable).

Once you do that - you can click on continue.

Do you want to continue? (yes/no): """

# Get user input
user_input = input(disclaimer).strip().lower()

# Check if the user wants to continue
if user_input == 'yes':
    # Call your firmware flashing tool here
    print("Starting firmware flashing tool...")
    esptool.main(['--chip', 'esp32', '--baud', '460800', '--before', 'default_reset', '--after', 'hard_reset', 'write_flash', '-z', '--flash_size', '4MB', '--flash_freq', '40m', '--flash_mode', 'dio', '0x1000', os.path.join(current_directory, 'firmware_full.bin')])
else:
    print("Firmware flashing canceled.")
    sys.exit(1)  # Exit the script with an error code if the user chooses not to continue

