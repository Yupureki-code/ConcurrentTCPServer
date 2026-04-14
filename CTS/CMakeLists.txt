file(GLOB SRC_LISTS "src/*.cpp")

add_library(CTS_static STATIC ${SRC_LISTS})
add_library(CTS_shared SHARED ${SRC_LISTS})

target_include_directories(CTS_static PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(CTS_shared PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(CTS_static PUBLIC pthread)
target_link_libraries(CTS_shared PUBLIC pthread)

set_target_properties(CTS_static PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME CTS
    VERSION 1.0.0
    SOVERSION 1
)

set_target_properties(CTS_shared PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME CTS
    VERSION 1.0.0
    SOVERSION 1
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS CTS_static CTS_shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY include/ 
        DESTINATION include/ConcurrentTCPServer
        FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")