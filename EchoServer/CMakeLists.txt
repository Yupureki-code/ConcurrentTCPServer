cmake_minimum_required(VERSION 3.28.3)

project(EchoServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CTS_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/../ConcurrentTCPServer/include)
set(CTS_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../ConcurrentTCPServer/output/lib)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/include)

add_library(EchoServer_static STATIC 
    src/EchoServer.cpp
)

add_library(EchoServer_shared SHARED 
    src/EchoServer.cpp
)

target_include_directories(EchoServer_static PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CTS_INCLUDE}
)

target_include_directories(EchoServer_shared PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CTS_INCLUDE}
)

target_link_libraries(EchoServer_static PUBLIC 
    CTS_static
    pthread
)

target_link_libraries(EchoServer_shared PUBLIC 
    CTS_shared
    pthread
)

set_target_properties(EchoServer_static PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME EchoServer
    VERSION 1.0.0
    SOVERSION 1
)

set_target_properties(EchoServer_shared PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME EchoServer
    VERSION 1.0.0
    SOVERSION 1
    POSITION_INDEPENDENT_CODE ON
    BUILD_RPATH "${CTS_LIB_DIR}"
    INSTALL_RPATH "${CTS_LIB_DIR}"
)

add_custom_command(TARGET EchoServer_static POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/output/include
)

add_custom_command(TARGET EchoServer_shared POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/output/include
)

install(TARGETS EchoServer_static EchoServer_shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/ 
        DESTINATION include/EchoServer
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")