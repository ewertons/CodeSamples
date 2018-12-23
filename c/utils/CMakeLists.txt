cmake_minimum_required(VERSION 2.8.11)

project(utils)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

get_filename_component(UTILS_FOLDER ${CMAKE_CURRENT_LIST_DIR} ABSOLUTE)
set(UTILS_FOLDER "${UTILS_FOLDER}" CACHE INTERNAL "" FORCE)
set(UTILS_INC_FOLDER ${UTILS_FOLDER}/inc CACHE INTERNAL "")

include_directories(${UTILS_INC_FOLDER})

set (header_files
    ./inc/logging.h
    ./inc/result.h
    ./inc/list.h
)

set (source_files 
    ./src/logging.c
    ./src/list.c
)

add_library(utils ${source_files} ${header_files})

#target_link_libraries(another-nas)

add_subdirectory(tests)
