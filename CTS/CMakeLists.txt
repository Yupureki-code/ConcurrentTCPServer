file(GLOB SRC_LISTS "src/*.cpp")

file(GLOB PUBLIC_HEADERS
    "include/*.h"
    "include/*.hpp"
)

set(PUBLIC_INCLUDE_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output/include")
file(MAKE_DIRECTORY "${PUBLIC_INCLUDE_OUTPUT_DIR}")
foreach(header ${PUBLIC_HEADERS})
    get_filename_component(header_name "${header}" NAME)
    configure_file("${header}" "${PUBLIC_INCLUDE_OUTPUT_DIR}/${header_name}" COPYONLY)
endforeach()

add_library(CTS_static STATIC ${SRC_LISTS})
add_library(CTS_shared SHARED ${SRC_LISTS})

target_include_directories(CTS_static PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Logger/include
)
target_include_directories(CTS_shared PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/Logger/include
)

target_link_libraries(CTS_static PUBLIC pthread Logger)
target_link_libraries(CTS_shared PUBLIC pthread Logger)

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

install(DIRECTORY ${PUBLIC_INCLUDE_OUTPUT_DIR}/
        DESTINATION include/CTS
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")