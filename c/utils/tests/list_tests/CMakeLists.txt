cmake_minimum_required(VERSION 2.8.11)

project(list_tests)

include_directories(${UTILS_INC_FOLDER})

set (header_files
)

set (source_files 
    ./main.c
)

add_executable(list_tests ${source_files} ${header_files})

target_link_libraries(list_tests
    utils
)
