cmake_minimum_required(VERSION 4.2.0)

project(
    the-blown-fuse
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A FUSE library for testing file-access failure scenarios"
)

set(CPACK_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-Wall -D_FILE_OFFSET_BITS=64")
set(CMAKE_MODULE_PATH, "${CMAKE_MODULE_PATH}")

include(GNUInstallDirs)

add_executable(bf_fuse)

find_package(Boost REQUIRED COMPONENTS program_options)

target_sources(bf_fuse
    PRIVATE
        src/bf_fuse.cxx
)

target_include_directories(bf_fuse PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(bf_fuse PRIVATE Boost::program_options)

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

include(CPack)