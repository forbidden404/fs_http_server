cmake_minimum_required(VERSION 3.14)

project(fs_http_serverTests LANGUAGES C)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(fs_http_server REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(fs_http_server_test source/fs_http_server_test.c)
target_link_libraries(fs_http_server_test PRIVATE fs_http_server::fs_http_server)
target_compile_features(fs_http_server_test PRIVATE c_std_23)

add_test(NAME fs_http_server_test COMMAND fs_http_server_test)

# ---- End-of-file commands ----

add_folders(Test)
