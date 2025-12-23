cmake_minimum_required(VERSION 4.2.0)

project(
    the-blown-fuse
    VERSION 0.0.1
    LANGUAGES C
    DESCRIPTION "A FUSE library for testing file-access failure scenarios"
)

set(CPACK_PACKAGE_LICENSE "GPL-3.0-or-later")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.md")

include(GNUInstallDirs)

add_executable(bf_fuse)
add_custom_target(docs)

target_sources(bf_fuse
    PRIVATE
        src/bf_fuse.c
)

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