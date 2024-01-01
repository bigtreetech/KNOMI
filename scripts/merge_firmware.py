Import("env")
import csv
import os

APP_BIN = os.path.join(env.subst("$BUILD_DIR") , "firmware.bin")
FS_BIN = os.path.join(env.subst("$BUILD_DIR") , "littlefs.bin")
MERGED_BIN = os.path.join(env.subst("$BUILD_DIR") , "firmware_full.bin")
BOARD_CONFIG = env.BoardConfig()

# Define function to parse partitions from CSV
def parse_partitions(filename):
    offsets = {}
    with open(filename, mode='r') as file:
        csv_reader = csv.reader(file)
        for row in csv_reader:
            if not row or row[0].startswith('#') or len(row) < 4:
                continue
            name = row[0].strip()
            offset = row[3].strip()
            offsets[name] = offset
    return offsets

def merge_firmware(*arg, **kwargs):
    offsets = parse_partitions("partition.csv")

     # The list contains all extra images (bootloader, partitions, eboot) and
    # the final application binary
    flash_images = env.Flatten(env.get("FLASH_EXTRA_IMAGES", [])) + [offsets["app0"], APP_BIN] + [offsets["app1"], APP_BIN] + [offsets["spiffs"], FS_BIN]

    def check_file_and_list_directory(path):
        if not os.path.isfile(path):
            print(f"The file '{path}' does not exist.")
            print(f"Directory contents of '{os.path.dirname(path)}':")
            for item in os.listdir(os.path.dirname(path)):
                print(item)

    print("Source images: ")
    for i in range(0, len(flash_images), 2):
        flash_images[i+1] = env.subst(flash_images[i+1])
        check_file_and_list_directory(flash_images[i+1])
        print(f"  {flash_images[i]} -> {flash_images[i+1]}")

    print("Target image: ")
    print(f"  {MERGED_BIN}")
    
    flash_size = env.BoardConfig().get("upload.flash_size", "4MB")

    flash_freq = env.BoardConfig().get("build.f_flash", "40000000L")
    flash_freq = str(flash_freq).replace("L", "")
    flash_freq = str(int(int(flash_freq) / 1000000)) + "m"
    flash_mode = env.BoardConfig().get("build.flash_mode", "dio")
    memory_type = env.BoardConfig().get("build.arduino.memory_type", "qio_qspi")

    if flash_mode == "qio" or flash_mode == "qout":
        flash_mode = "dio"
    if memory_type == "opi_opi" or memory_type == "opi_qspi":
        flash_mode = "dout"
    # Run esptool to merge images into a single binary
    env.Execute(
        " ".join(
            [
                "$PYTHONEXE",
                "$OBJCOPY",
                "--chip",
                BOARD_CONFIG.get("build.mcu", "esp32"),
                "merge_bin",
                "-o",
                MERGED_BIN,
                "--flash_mode",
                flash_mode,
                "--flash_size",
                flash_size,
                "--target-offset",
                "0x1000"
            ]
            + flash_images
        )
    )

env.AddCustomTarget(
    "merge_firmware",
    None,
    merge_firmware,
    title="merge_firmware",
    description="Merge firmware files into single binary")