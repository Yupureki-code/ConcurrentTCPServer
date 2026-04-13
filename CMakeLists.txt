cmake_minimum_required(VERSION 3.28.3)

project(ConcurrentTCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "Build tests" ON)

include_directories(${CMAKE_SOURCE_DIR}/ConcurrentTCPServer/include)

add_subdirectory(ConcurrentTCPServer)
add_subdirectory(EchoServer)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

install(DIRECTORY ConcurrentTCPServer/include/ 
        DESTINATION include/ConcurrentTCPServer
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

install(DIRECTORY EchoServer/include/ 
        DESTINATION include/EchoServer
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")