include(CMakePackageConfigHelpers)

if(NOT TARGET Logger::Logger_static)
    find_package(Logger CONFIG REQUIRED)
endif()

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

add_library(CTS::CTS_static ALIAS CTS_static)
add_library(CTS::CTS_shared ALIAS CTS_shared)

target_include_directories(CTS_static PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PUBLIC_INCLUDE_OUTPUT_DIR}>
    $<INSTALL_INTERFACE:include>
)
target_include_directories(CTS_shared PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PUBLIC_INCLUDE_OUTPUT_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(CTS_static PUBLIC pthread Logger::Logger_static)
target_link_libraries(CTS_shared PUBLIC pthread Logger::Logger_shared)

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
        EXPORT CTSTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY ${PUBLIC_INCLUDE_OUTPUT_DIR}/
        DESTINATION include/CTS
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

install(EXPORT CTSTargets
        FILE CTSTargets.cmake
        NAMESPACE CTS::
        DESTINATION lib/cmake/CTS)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/CTSConfigVersion.cmake
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/CTSConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/CTSConfig.cmake
    INSTALL_DESTINATION lib/cmake/CTS
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/CTSConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/CTSConfigVersion.cmake
    DESTINATION lib/cmake/CTS
)