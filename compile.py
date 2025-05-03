import os
import subprocess
import sys

# Constants
SOURCE_FILE = "main.cpp"
OUTPUT_DIR = ".\\bin"

CLANG_DEBUG_FLAGS = ["-O0", "-std=c++23" , "-Wno-unused-but-set-variable"]
CLANG_RELEASE_FLAGS = ["-O3", "-std=c++23", "-Wno-unused-but-set-variable"]
CLANG_IR = ["-O3", "-std=c++23", "-S", "-emit-llvm", "-Wno-unused-but-set-variable"]

MSVC_DEBUG_FLAGS = ["/Od", "/Zi", "/std:c++latest"]
MSVC_RELEASE_FLAGS = ["/O2", "/std:c++latest"]

# Ensure output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

def run_command(cmd) -> None:
    print(f"Running: {' '.join(cmd)}")
    result = subprocess.run(cmd)
    if result.returncode != 0:
        print("Compilation failed.")
        sys.exit(result.returncode)

def compile_with_clang(output_name, flags) -> None:
    os.makedirs(OUTPUT_DIR+"_CLANG", exist_ok=True)
    base_cmd = ["clang++", SOURCE_FILE, "-o", os.path.join(OUTPUT_DIR+"_CLANG", output_name)]
    cmd = base_cmd + flags
    run_command(cmd)

def compile_with_msvc(output_name, flags) -> None:
    # cl.exe requires environment variables set by Developer Command Prompt
    # So this script must be run inside Developer Command Prompt or with proper setup
    os.makedirs(OUTPUT_DIR+"_MSVC", exist_ok=True)
    base_cmd = ["cl", SOURCE_FILE, "/EHsc", "/nologo", "/Fe:" + os.path.join(OUTPUT_DIR+"_MSVC", output_name)]
    cmd = base_cmd + [f for f in flags] + ["/Fo" + os.path.join(OUTPUT_DIR+"_MSVC", "obj_") + output_name.replace(".exe", ".obj")]
    run_command(cmd)

if __name__ == "__main__":
    print("🚀 Starting custom build process...\n")

    print("🗂️ Creating output directory...")
    print()

    # Compile with Clang
    print("⚙️ Compiling with Clang")
    try:
        compile_with_clang("Clang_NO_OPT.exe", CLANG_DEBUG_FLAGS)
        compile_with_clang("Clang_OPT3.exe", CLANG_RELEASE_FLAGS)
        compile_with_clang("Clang_OPT3_IR.ll", CLANG_IR)
        print("✅ Clang builds completed\n")
    except Exception as e:
        print("❌ Failed to compile with Clang:", str(e))

    # Compile with MSVC
    print("⚙️ Compiling with MSVC")
    try:
        compile_with_msvc("MSVC_NO_OPT.exe", MSVC_DEBUG_FLAGS)
        compile_with_msvc("MSVC_OPT3.exe", MSVC_RELEASE_FLAGS)
        print("✅ MSVC builds completed\n")
    except Exception as e:
        print("❌ Failed to compile with MSVC:", str(e))

    print(f"📂 Binaries saved to {OUTPUT_DIR}\\")
    print("🎉 Done!")
