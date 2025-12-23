cmake_minimum_required(VERSION 4.2.0)

project(
    the-blown-fuse
    VERSION 0.0.1
    LANGUAGES C
    DESCRIPTION "A FUSE library for testing file-access failure scenarios"
)

include(GNUInstallDirs)

add_executable(bf_fuse)

target_sources(bf_fuse
    PRIVATE
        src/bf_fuse.c
)

install(
    TARGETS bf_fuse
    
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
)