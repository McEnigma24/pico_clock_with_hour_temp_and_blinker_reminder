cmake_minimum_required(VERSION 3.18)
include(pico_sdk_import.cmake)
project(MyProject LANGUAGES C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(PICO_EXAMPLES_PATH ${PROJECT_SOURCE_DIR})
pico_sdk_init()

include_directories(_inc)
aux_source_directory(_src SOURCES)

set(EXE_NAME "out.exe")
add_executable(${EXE_NAME} ${SOURCES})
target_compile_definitions(${EXE_NAME} PRIVATE PICO)

target_link_libraries(${EXE_NAME} pico_stdlib)

pico_add_extra_outputs(${EXE_NAME})
