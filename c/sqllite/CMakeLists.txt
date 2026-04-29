cmake_minimum_required(VERSION 3.15)
project(sqlite_sample C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(sqlite_sample main.c)

# Try to use a system-installed SQLite first (libsqlite3-dev on Ubuntu,
# vcpkg / system package on Windows). If not found, fall back to pkg-config.
find_package(SQLite3 QUIET)

if(SQLite3_FOUND)
    message(STATUS "Using system SQLite3: ${SQLite3_VERSION}")
    target_link_libraries(sqlite_sample PRIVATE SQLite::SQLite3)
else()
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(SQLITE3 REQUIRED sqlite3)
        target_include_directories(sqlite_sample PRIVATE ${SQLITE3_INCLUDE_DIRS})
        target_link_libraries(sqlite_sample PRIVATE ${SQLITE3_LIBRARIES})
        target_link_directories(sqlite_sample PRIVATE ${SQLITE3_LIBRARY_DIRS})
    else()
        message(FATAL_ERROR
            "SQLite3 not found. Install libsqlite3-dev (Linux) "
            "or sqlite3 via vcpkg (Windows).")
    endif()
endif()

if(MSVC)
    target_compile_options(sqlite_sample PRIVATE /W4)
else()
    target_compile_options(sqlite_sample PRIVATE -Wall -Wextra)
endif()
