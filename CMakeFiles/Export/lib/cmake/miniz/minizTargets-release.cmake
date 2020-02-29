#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "miniz::miniz" for configuration "Release"
set_property(TARGET miniz::miniz APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(miniz::miniz PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libminiz.so.2.1.0"
  IMPORTED_SONAME_RELEASE "libminiz.so.2"
  )

list(APPEND _IMPORT_CHECK_TARGETS miniz::miniz )
list(APPEND _IMPORT_CHECK_FILES_FOR_miniz::miniz "${_IMPORT_PREFIX}/lib/libminiz.so.2.1.0" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
