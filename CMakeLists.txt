cmake_minimum_required(VERSION 3.28.3)

project(ConcurrentTCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

option(BUILD_TESTING "Build tests" ON)
option(BUILD_ECHOSERVER "Build EchoServer" OFF)
option(BUILD_LEGACY_TESTS "Build legacy CMake tests" OFF)
option(BUILD_HTTPSERVER_TESTS "Build HttpServer tests" ON)

include_directories(${CMAKE_SOURCE_DIR}/CTS/include)

add_subdirectory(Logger)
add_subdirectory(CTS)

if(BUILD_ECHOSERVER)
    add_subdirectory(EchoServer)
endif()

if(BUILD_TESTING)
    enable_testing()
endif()

if(BUILD_TESTING AND BUILD_LEGACY_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_TESTING AND BUILD_HTTPSERVER_TESTS)
    add_subdirectory(HttpServer/test)
endif()