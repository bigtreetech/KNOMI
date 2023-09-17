Import("env")
import subprocess
import os

def linter_callback(*arg, **kwargs):
    print("Executing Formatter")

    check_folders = [env.get("PROJECT_INCLUDE_DIR"), env.get("PROJECT_SRC_DIR")]
    
    project_dir = env.get("PROJECT_DIR") + "\\"
    check_folders_without_prefix = [str(i).replace( project_dir, '') for i in check_folders]

    print("check folders", check_folders_without_prefix);
    
    subprocess.run("\"$PYTHONEXE\" -m pip -q install clang-format")
    files = []
    for folder in check_folders_without_prefix:
        # r=root, d=directories, f = files
        for r, d, f in os.walk(folder):
            for file in f:
                files.append(os.path.join(r, file))

    files = [ file for file in files if file.endswith( ('.c','.cpp', '.h', '.hpp', '.cc', '.cxx', '.hxx','.hh') ) ]

    for file in files:
        env.Execute("clang-format -i "+ file)

env.AddCustomTarget(
    "format",
    None,
    linter_callback,
    title="clang-format",
    description="Executes Source Code Formatter")

propName = "custom_clangFormat"
default = env.GetProjectConfig().get("env", propName)

if not BUILD_TARGETS and env.GetProjectOption(propName, default) == "true":
    linter_callback();