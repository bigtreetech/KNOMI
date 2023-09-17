import datetime
import subprocess

Import("env")

def generate_version(*arg, **kwargs):
    VERSION_FILE = 'version.cpp'

    result = subprocess.run(['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")
    branch = subprocess.run(['git', 'rev-parse', '--abbrev-ref', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")
    commitTime = subprocess.run(['git', 'show', '-s', '--format=%ci', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")

    VERSION_CONTENTS = """#include "../Version.h"

String Version::getGitCommitSha1() {
    return "$1";
}
String Version::getGitBranch() {
    return "$2";
}
String Version::getGitTimestamp() {
    return "$3";
}
String Version::getBuildTimestamp() {
    return "$4";
}
    """.replace("$1", result).replace("$2", branch).replace("$3", commitTime).replace("$4", datetime.datetime.now().strftime("%m/%d/%Y, %H:%M:%S"))

    VERSION_FILE = "src/generated/Version.cpp"

    print("Updating {} with version/timestamp...".format(VERSION_FILE))
    with open(VERSION_FILE, 'w+') as FILE:
        FILE.write(VERSION_CONTENTS)


env.AddCustomTarget(
    "generate_version",
    None,
    generate_version,
    title="generate_version",
    description="Generate version.h")

propName = "custom_generateVersion"
default = env.GetProjectConfig().get("env", propName)

print("Current Build targets $PROGPATH")

if not BUILD_TARGETS and env.GetProjectOption(propName, default) == "true":
    generate_version();