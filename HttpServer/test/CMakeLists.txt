set(HTTPSERVER_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/bad_request_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/business_timeout_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/large_file_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/long_connection_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/parallel_requests_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/pressure_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/request_timeout_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/timeout_connection_test.cpp
)

set(HTTPSERVER_TEST_TARGETS
    httpserver_bad_request_test
    httpserver_business_timeout_test
    httpserver_large_file_test
    httpserver_long_connection_test
    httpserver_parallel_requests_test
    httpserver_pressure_test
    httpserver_request_timeout_test
    httpserver_timeout_connection_test
)
set(HTTPSERVER_TEST_PORT 18081)

add_executable(httpserver_test_server
    test.cpp
)

set_target_properties(httpserver_test_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_definitions(httpserver_test_server PRIVATE
    HTTP_SERVER_TEST_PORT=${HTTPSERVER_TEST_PORT}
    HTTP_SERVER_WWWROOT="${CMAKE_SOURCE_DIR}/HttpServer/src/wwwroot"
)

target_include_directories(httpserver_test_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
)

target_link_libraries(httpserver_test_server PRIVATE
    CTS_static
    Logger_static
    pthread
)

foreach(index RANGE 0 7)
    list(GET HTTPSERVER_TEST_SOURCES ${index} source)
    list(GET HTTPSERVER_TEST_TARGETS ${index} target_name)
    add_executable(${target_name} ${source})
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_compile_definitions(${target_name} PRIVATE
        HTTP_SERVER_TEST_PORT=${HTTPSERVER_TEST_PORT}
        HTTP_SERVER_WWWROOT="${CMAKE_SOURCE_DIR}/HttpServer/src/wwwroot"
    )
    target_include_directories(${target_name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
        ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    target_link_libraries(${target_name} PRIVATE
        CTS_static
        Logger_static
        pthread
    )
endforeach()

add_custom_target(httpserver_run_cpp
    COMMAND /bin/sh ${CMAKE_CURRENT_SOURCE_DIR}/run_httpserver_tests.sh
    DEPENDS httpserver_test_server ${HTTPSERVER_TEST_TARGETS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Run HttpServer C++ tests"
)

if(BUILD_TESTING)
    add_test(NAME httpserver_cpp_suite COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target httpserver_run_cpp)
endif()
