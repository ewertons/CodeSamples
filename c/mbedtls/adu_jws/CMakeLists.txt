cmake_minimum_required(VERSION 3.5.1)

project("mbedtest" C)

option(ENABLE_TESTING OFF)
add_subdirectory(mbedtls)

add_executable(mbedtest
    ${CMAKE_CURRENT_LIST_DIR}/main.c
)

target_link_libraries(mbedtest
    PUBLIC
        mbedtls
)


