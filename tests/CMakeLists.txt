file(GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

set(CTS_INCLUDE ${CMAKE_SOURCE_DIR}/ConcurrentTCPServer/include)
set(ECHO_INCLUDE ${CMAKE_SOURCE_DIR}/EchoServer/include)

foreach(source ${TEST_SOURCES})
    get_filename_component(test_name ${source} NAME_WE)
    add_executable(${test_name} ${source})
    target_include_directories(${test_name} PRIVATE 
        ${ECHO_INCLUDE}
        ${CTS_INCLUDE}
    )
    target_link_libraries(${test_name} 
        CTS_static 
        EchoServer_static
        pthread 
    )
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()