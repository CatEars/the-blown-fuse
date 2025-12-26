import sys, re
from pathlib import Path

def main():
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

    sys.exit(0)

if __name__ == "__main__":
    main()
