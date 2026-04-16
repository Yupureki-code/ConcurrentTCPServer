#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "CTS::CTS_static" for configuration ""
set_property(TARGET CTS::CTS_static APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CTS::CTS_static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libCTS.a"
  )

list(APPEND _cmake_import_check_targets CTS::CTS_static )
list(APPEND _cmake_import_check_files_for_CTS::CTS_static "${_IMPORT_PREFIX}/lib/libCTS.a" )

# Import target "CTS::CTS_shared" for configuration ""
set_property(TARGET CTS::CTS_shared APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(CTS::CTS_shared PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libCTS.so.1.0.0"
  IMPORTED_SONAME_NOCONFIG "libCTS.so.1"
  )

list(APPEND _cmake_import_check_targets CTS::CTS_shared )
list(APPEND _cmake_import_check_files_for_CTS::CTS_shared "${_IMPORT_PREFIX}/lib/libCTS.so.1.0.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
