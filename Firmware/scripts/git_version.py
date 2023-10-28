import datetime
import subprocess
import json

Import("env")

def generate_version(*arg, **kwargs):
    try:
        from dateutil import parser
    except ImportError:
        env.Execute("\"$PYTHONEXE\" -m pip install python-dateutil~=2.8.2")
        from dateutil import parser

    try:
        import pytz
    except ImportError:
        env.Execute("\"$PYTHONEXE\" -m pip install pytz~=2023.3.post1")
        import pytz

    result = subprocess.run(['git', 'rev-parse', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")
    branch = subprocess.run(['git', 'rev-parse', '--abbrev-ref', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")
    commitTime = subprocess.run(['git', 'show', '-s', '--format=%ci', 'HEAD'], stdout=subprocess.PIPE).stdout.decode('utf-8').replace("\n", "").replace("\r", "")
    timeformat = "%Y-%m-%dT%H:%M:%S.%f%z";
    
    commitTime = parser.parse(commitTime).astimezone(pytz.utc).strftime(timeformat);
    buildTime = datetime.datetime.utcnow().astimezone(pytz.utc).strftime(timeformat);

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
    """.replace("$1", result).replace("$2", branch).replace("$3", commitTime).replace("$4", buildTime)

    VERSION_FILE = "src/generated/Version.cpp"
    VERSION_JSON = env.subst("$BUILD_DIR") + '/buildinfo.json'

    print("Updating {} with version/timestamp...".format(VERSION_FILE))
    with open(VERSION_FILE, 'w+') as FILE:
        FILE.write(VERSION_CONTENTS)

    data = {
        "commit": result,
        "branch": branch,
        "commitTime": commitTime,
        "buildTime": buildTime
    }

    with open(VERSION_JSON, 'w+') as FILE:
        json.dump(data, FILE)

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