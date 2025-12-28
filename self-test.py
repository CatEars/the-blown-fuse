import sys, re
from pathlib import Path

def all_tests_are_run_in_cmake():
    cmake_path = Path.cwd() / "CMakeLists.txt"
    if not cmake_path.is_file():
        print("CMakeLists.txt not found")
        sys.exit(1)
    
    text = cmake_path.read_text()
    matches = re.findall(r'add_boost_test\s*\(([^)]*)\)', text, flags=re.IGNORECASE)
    cmake_tests = set()
    for m in matches:
        parts = [p.strip() for p in m.split() if p.strip()]
        cmake_tests.update(parts)
    src_dir = Path.cwd() / "src"
    if not src_dir.is_dir():
        print("src directory not found")
        sys.exit(1)
    
    src_tests = set()
    for p in src_dir.iterdir():
        if p.is_file() and p.name.startswith("test_"):
            src_tests.add(str(p.relative_to(Path.cwd())))
    missing_in_src = cmake_tests - src_tests
    extra_in_src = src_tests - cmake_tests
    for t in sorted(missing_in_src):
        print("Missing in src:", t)
    for t in sorted(extra_in_src):
        print("Unlinked test file:", t)

    if missing_in_src or extra_in_src:
        sys.exit(1)


def all_headers_uses_pragma_once():
    path = Path.cwd() / 'src'
    any_missing = False

    for hpp_file in path.rglob('**/*.hpp'):
        try:
            with hpp_file.open('r', encoding='utf-8') as f:
                has_pragma = any(line.strip().startswith('#pragma once') for line in f)
                
                if not has_pragma:
                    print(f"MISSING: {hpp_file}")
                    any_missing = True
                else:
                    print(f"OK:      {hpp_file}")
                    
        except (UnicodeDecodeError, PermissionError) as e:
            print(f"ERROR: Could not read {hpp_file} - {e}")
    
    if any_missing:
        sys.exit(1)


def all_cpp_files_start_with_license_header():
    src_directory = Path.cwd() / 'src'
    license_file_path = Path.cwd() / 'data' / 'license-header.txt'
    license_path = Path(license_file_path)
    if not license_path.exists():
        print(f"Error: License file not found at {license_file_path}")
        return

    license_text = license_path.read_text(encoding='utf-8').strip()
    src_path = Path(src_directory)
    extensions = {'.hpp', '.cxx'}
    
    missing_count = 0
    total_files = 0

    for file_path in src_path.rglob('**/*'):
        if file_path.suffix in extensions:
            total_files += 1
            try:
                content = file_path.read_text(encoding='utf-8').strip()
                
                if not content.startswith(license_text):
                    print(f"[-] MISSING/MISMATCH: {file_path}")
                    missing_count += 1
                else:
                    print(f"[+] VALID:            {file_path}")
                    
            except Exception as e:
                print(f"[!] ERROR reading {file_path}: {e}")

    print(f"\nScan complete. {missing_count}/{total_files} files failed the check.")

    if missing_count > 0:
        sys.exit(1)


def main():
    print('### Ensuring all tests are run in CMake')
    all_tests_are_run_in_cmake()
    print('### OK')
    print('### Ensuring all header files have "#pragma once"')
    all_headers_uses_pragma_once()
    print('### OK')
    print('### Ensuring all files have license text')
    all_cpp_files_start_with_license_header()
    print('### OK')
    sys.exit(0)

if __name__ == "__main__":
    main()
