if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/fs_http_server-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package fs_http_server)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT fs_http_server_Development
)

install(
    TARGETS fs_http_server_fs_http_server
    EXPORT fs_http_serverTargets
    RUNTIME #
    COMPONENT fs_http_server_Runtime
    LIBRARY #
    COMPONENT fs_http_server_Runtime
    NAMELINK_COMPONENT fs_http_server_Development
    ARCHIVE #
    COMPONENT fs_http_server_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    fs_http_server_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE fs_http_server_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(fs_http_server_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${fs_http_server_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT fs_http_server_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${fs_http_server_INSTALL_CMAKEDIR}"
    COMPONENT fs_http_server_Development
)

install(
    EXPORT fs_http_serverTargets
    NAMESPACE fs_http_server::
    DESTINATION "${fs_http_server_INSTALL_CMAKEDIR}"
    COMPONENT fs_http_server_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
