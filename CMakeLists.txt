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

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.tar.gz
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(OrderBookTests tests/OrderBookTests.cpp src/OrderBook.cpp)
target_include_directories(OrderBookTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(OrderBookTests PRIVATE GTest::gtest_main)
