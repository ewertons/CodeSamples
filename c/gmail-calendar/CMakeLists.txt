cmake_minimum_required(VERSION 3.18)
project(gcal_c_sample C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# -----------------------------------------------------------------------------
# Dependencies
#
# Pulled in via FetchContent so the sample is self-contained. First configure
# is slow (downloads + builds OpenSSL-linked http-c).
# -----------------------------------------------------------------------------
include(FetchContent)

# http-c (HTTPS client/server library; depends on OpenSSL + cmocka indirectly).
# We consume the `http` and `common-lib-c` CMake targets. common-lib-c
# provides both span_t / result_t and the json_reader_t parser.
FetchContent_Declare(
    http_c
    GIT_REPOSITORY https://github.com/ewertons/http-c.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(http_c)

# -----------------------------------------------------------------------------
# Sample target
# -----------------------------------------------------------------------------
find_package(OpenSSL REQUIRED)

add_executable(gcal_c_sample
    src/main.c
    src/oauth.c
    src/loopback.c
    src/calendar.c
    src/util.c
    src/http_helper.c
)

target_include_directories(gcal_c_sample PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${http_c_SOURCE_DIR}/inc
    ${http_c_SOURCE_DIR}/deps/common-lib-c/inc
)

target_link_libraries(gcal_c_sample PRIVATE
    http
    common-lib-c
    OpenSSL::SSL
    OpenSSL::Crypto
    pthread
)

target_compile_options(gcal_c_sample PRIVATE
    -Wall -Wextra -Wpedantic
)
