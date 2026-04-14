cmake_minimum_required(VERSION 3.28.3)

project(ConcurrentTCPServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

option(BUILD_TESTING "Build tests" ON)

include_directories(${CMAKE_SOURCE_DIR}/CTS/include)

add_subdirectory(CTS)
#add_subdirectory(EchoServer)

if(BUILD_TESTING)
    enable_testing()
    #add_subdirectory(tests)
endif()

install(DIRECTORY CTS/include/ 
        DESTINATION include/CTS
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

#install(DIRECTORY EchoServer/include/ 
        #DESTINATION include/EchoServer
       # FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")