cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(sqlite3 LANGUAGES C)

set(SQLITE_SOURCE
        sqlite/sqlite3.h
        sqlite/sqlite3ext.h
        sqlite/sqlite3.c
)

add_library(sqlite3 STATIC ${SQLITE_SOURCE})
add_library(SQLite3::SQLite3 ALIAS sqlite3)


target_include_directories(sqlite3
        PUBLIC sqlite/
)

if (UNIX)
    find_package(Threads REQUIRED)

    target_link_libraries(sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})
endif()
