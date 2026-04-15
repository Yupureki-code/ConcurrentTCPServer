file(GLOB LOGGER_SRC "src/*.cpp")

file(GLOB LOGGER_PUBLIC_HEADERS
    "include/*.h"
    "include/*.hpp"
)

set(LOGGER_PUBLIC_INCLUDE_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/output/include")
file(MAKE_DIRECTORY "${LOGGER_PUBLIC_INCLUDE_OUTPUT_DIR}")
foreach(header ${LOGGER_PUBLIC_HEADERS})
    get_filename_component(header_name "${header}" NAME)
    configure_file("${header}" "${LOGGER_PUBLIC_INCLUDE_OUTPUT_DIR}/${header_name}" COPYONLY)
endforeach()

add_library(Logger_static STATIC ${LOGGER_SRC})
add_library(Logger_shared SHARED ${LOGGER_SRC})

target_include_directories(Logger_static PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(Logger_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(Logger_static PUBLIC pthread)
target_link_libraries(Logger_shared PUBLIC pthread)

set_target_properties(Logger_static PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME Logger
    VERSION 1.0.0
    SOVERSION 1
)

set_target_properties(Logger_shared PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output/lib
    OUTPUT_NAME Logger
    VERSION 1.0.0
    SOVERSION 1
    POSITION_INDEPENDENT_CODE ON
)

install(TARGETS Logger_static Logger_shared
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY ${LOGGER_PUBLIC_INCLUDE_OUTPUT_DIR}/
        DESTINATION include/Logger
        FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")