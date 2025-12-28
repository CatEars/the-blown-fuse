import subprocess
import datetime
import sys
import shutil
import re
from pathlib import Path

def get_git_decimal_hash():
    try:
        short_hash = subprocess.check_output(['git', 'rev-parse', '--short', 'HEAD']).decode().strip()
        return str(int(short_hash, 16))
    except Exception:
        sys.exit(1)

def update_file(filepath, pattern, replacement):
    content = filepath.read_text()
    new_content = re.sub(pattern, replacement, content, flags=re.IGNORECASE)
    filepath.write_text(new_content)

def run_command(command):
    result = subprocess.run(command, shell=True)
    return result.returncode

def verify_binary_version(version_str):
    binary_path = Path.cwd() / "build" / "bf_fuse"
    try:
        output = subprocess.check_output([str(binary_path), "--version"], stderr=subprocess.STDOUT).decode()
        expected = f"bf_fuse version {version_str}"
        if expected not in output:
            print(f"Version mismatch! Expected: '{expected}', Got: '{output.strip()}'")
            return False
        return True
    except Exception as e:
        print(f"Failed to run binary version check: {e}")
        return False

def main():
    now = datetime.datetime.now()
    decimal_hash = get_git_decimal_hash()
    version_str = f"{now.year}.{now.month}.{decimal_hash}"
    
    root_dir = Path.cwd()
    cmake_file = root_dir / "CMakeLists.txt"
    version_header = root_dir / "src" / "version.hpp"
    dist_dir = root_dir / "dist"
    build_dir = root_dir / "build"

    update_file(cmake_file, r'BF_FUSE_VERSION \d+\.\d+\.\d+', f'BF_FUSE_VERSION {version_str}')
    update_file(version_header, r'BF_FUSE_VERSION "\d+\.\d+\.\d+"', f'BF_FUSE_VERSION "{version_str}"')

    try:
        if run_command("make self-test") != 0:
            raise Exception()
        
        run_command("make clean")
        
        if run_command("make test") != 0:
            run_command(f"git checkout {cmake_file} {version_header}")
            sys.exit(1)

        if run_command("make package") != 0:
            raise Exception()

        if not verify_binary_version(version_str):
            raise Exception()

        dist_dir.mkdir(parents=True, exist_ok=True)

        installer_name = f"the-blown-fuse-{version_str}-Linux.sh"
        source_path = build_dir / installer_name
        
        if source_path.exists():
            shutil.copy(source_path, dist_dir / installer_name)
            run_command(f"git tag {version_str}")
        else:
            print('[Error] could not find expected output file:', installer_name)
            raise Exception()

        print("reverting version changes")
        run_command(f"git checkout {cmake_file} {version_header}")
    except Exception:
        run_command(f"git checkout {cmake_file} {version_header}")
        sys.exit(1)

    
if __name__ == "__main__":
    main()