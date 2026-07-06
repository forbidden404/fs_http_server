cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
    fs_http_server
    VERSION 0.1.0
    DESCRIPTION "A simple HTTP server written in C"
    LANGUAGES C
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    fs_http_server_fs_http_server
    source/fs_http_server.c
)
add_library(fs_http_server::fs_http_server ALIAS fs_http_server_fs_http_server)

include(GenerateExportHeader)
generate_export_header(
    fs_http_server_fs_http_server
    BASE_NAME fs_http_server
    EXPORT_FILE_NAME export/fs_http_server/fs_http_server_export.h
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(fs_http_server_fs_http_server PUBLIC FS_HTTP_SERVER_STATIC_DEFINE)
endif()

set_target_properties(
    fs_http_server_fs_http_server PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME fs_http_server
    OUTPUT_NAME fs_http_server
)

target_include_directories(
    fs_http_server_fs_http_server ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    fs_http_server_fs_http_server SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_compile_features(fs_http_server_fs_http_server PUBLIC c_std_23)
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT fs_http_server_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of fs_http_server"
  )
endif()

include(cmake/dev-mode.cmake)
