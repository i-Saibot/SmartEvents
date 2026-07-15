cmake_minimum_required(VERSION 3.10)
project(SmartEvents)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fPIC -O3 -w -DLINUX -fno-plt -fpermissive")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -static-libgcc -static-libstdc++")

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartEvents/include
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartEvents/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartEvents/lib/samp-sdk
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartEvents/lib/samp-sdk/amx
    ${CMAKE_CURRENT_SOURCE_DIR}/SmartEvents/lib/SQLiteCpp/include
)

file(GLOB_RECURSE SRC_FILES "SmartEvents/src/*.cpp")
file(GLOB_RECURSE SDK_FILES "SmartEvents/lib/samp-sdk/*.cpp")
file(GLOB_RECURSE SQLITE_FILES "SmartEvents/lib/SQLiteCpp/src/*.cpp")

set(SOURCES ${SRC_FILES} ${SDK_FILES} ${SQLITE_FILES})

message(STATUS "--- Found sources in: ${SOURCES}")

if(NOT SOURCES)
    message(FATAL_ERROR "SOURCES list is empty! Check folders in SmartEvents/...")
endif()

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_link_libraries(${PROJECT_NAME} sqlite3)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES 
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Release"
)