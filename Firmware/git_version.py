import datetime
import os
import subprocess

Import("env")

VERSION_FILE = 'version.cpp'

result = subprocess.run(['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")

VERSION_CONTENTS = """#include "version.h"

String Version::getGitCommitSha1() {
 return "$1";
}
String Version::getBuildTimestamp() {
 return "$2";
}
""".replace("$1", result).replace("$2", datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

VERSION_FILE = "src/generated/Version.cpp"

print("Updating {} with version/timestamp...".format(VERSION_FILE))
with open(VERSION_FILE, 'w+') as FILE:
        FILE.write(VERSION_CONTENTS)