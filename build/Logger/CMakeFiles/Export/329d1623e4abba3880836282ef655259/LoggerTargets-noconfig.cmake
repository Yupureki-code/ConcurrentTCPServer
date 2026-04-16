#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Logger::Logger_static" for configuration ""
set_property(TARGET Logger::Logger_static APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Logger::Logger_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libLogger.a"
  )

list(APPEND _cmake_import_check_targets Logger::Logger_static )
list(APPEND _cmake_import_check_files_for_Logger::Logger_static "${_IMPORT_PREFIX}/lib/libLogger.a" )

# Import target "Logger::Logger_shared" for configuration ""
set_property(TARGET Logger::Logger_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Logger::Logger_shared PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libLogger.so.1.0.0"
  IMPORTED_SONAME_NOCONFIG "libLogger.so.1"
  )

list(APPEND _cmake_import_check_targets Logger::Logger_shared )
list(APPEND _cmake_import_check_files_for_Logger::Logger_shared "${_IMPORT_PREFIX}/lib/libLogger.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
