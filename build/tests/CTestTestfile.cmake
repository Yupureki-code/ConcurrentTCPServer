# CMake generated Testfile for 
# Source directory: /home/yupureki/project/ConcurrentTCPServer/tests
# Build directory: /home/yupureki/project/ConcurrentTCPServer/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[repro_release_race]=] "/home/yupureki/project/ConcurrentTCPServer/build/tests/repro_release_race")
set_tests_properties([=[repro_release_race]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;18;add_test;/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;0;")
add_test([=[test_buffer]=] "/home/yupureki/project/ConcurrentTCPServer/build/tests/test_buffer")
set_tests_properties([=[test_buffer]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;18;add_test;/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;0;")
add_test([=[timer_block_repro]=] "/home/yupureki/project/ConcurrentTCPServer/build/tests/timer_block_repro")
set_tests_properties([=[timer_block_repro]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;18;add_test;/home/yupureki/project/ConcurrentTCPServer/tests/CMakeLists.txt;0;")
