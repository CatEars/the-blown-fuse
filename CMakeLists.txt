cmake_minimum_required(VERSION 4.2.0)

project(
    the-blown-fuse
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A FUSE library for testing file-access failure scenarios"
)

### Configuration
set(CPACK_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -D_FILE_OFFSET_BITS=64")
set(CMAKE_MODULE_PATH, "${CMAKE_MODULE_PATH}")

set(BF_FUSE_LINK_DEPENDENCIES
    Boost::program_options
    PkgConfig::FUSE3)

### CMake library
include(GNUInstallDirs)
include(CTest)
include(${CMAKE_SOURCE_DIR}/cmake-modules/AddBoostTest.cmake)

### Program dependencies
find_package(Boost REQUIRED COMPONENTS program_options unit_test_framework)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FUSE3 REQUIRED IMPORTED_TARGET fuse3)

### Program Testing
add_boost_test(src/test_arguments.cxx)
add_boost_test(src/test_plan_file_operations.cxx)
add_boost_test(src/test_parse_environment.cxx)

### Program definition
add_executable(bf_fuse)

target_sources(bf_fuse
    PRIVATE
        src/bf_fuse.cxx
)

target_include_directories(bf_fuse PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(bf_fuse PRIVATE ${BF_FUSE_LINK_DEPENDENCIES})

### Program distribution
install(
    TARGETS bf_fuse
    
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
    
    PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE
        GROUP_READ GROUP_EXECUTE
        WORLD_READ WORLD_EXECUTE
)

install(
    FILES
        LICENSE.md
        README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
)

### Build distributable
include(CPack)
