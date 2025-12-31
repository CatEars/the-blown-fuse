#!/bin/bash

THIS_SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

$THIS_SCRIPT_DIR/setup-git-aliases.sh

# Install boost and fuse
sudo apt update && sudo apt install -y libboost-all-dev libfuse3-dev

# Install clang
wget -qO- https://apt.llvm.org/llvm.sh | sudo bash -s -- 18

# Install CMake
wget -O /tmp/install-cmake.sh https://github.com/Kitware/CMake/releases/download/v4.2.1/cmake-4.2.1-linux-x86_64.sh
chmod +x /tmp/install-cmake.sh
sudo /tmp/install-cmake.sh --skip-license --prefix=/usr
