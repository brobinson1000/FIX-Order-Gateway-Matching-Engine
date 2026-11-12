cmake_minimum_required(VERSION 3.20)
project(FIXLOBENGINE VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(SOURCES
    src/main.cpp
    src/OrderBook.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    message(FATAL_ERROR "THIS PROJECT IS BEING RAN ON MSVC. PLEASE RECOMPILE ON LINUX")
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

