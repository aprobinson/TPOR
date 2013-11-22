# Configure the Boost options that will be required in this project. Optional
# arguments are the boost libraries that you would like to find. The
# following variables will be set:
# 1.) Boost_LIBRARIES
MACRO(ENABLE_BOOST_SUPPORT)
  
  # Only use static libraries
  SET(Boost_USE_STATIC_LIBS ON)
  SET(Boost_USE_STATIC_RUNTIME ON)
  
  # No multithreading
  SET(Boost_USE_MULTITHREAD OFF)

  # Use the user supplied prefix to find the Boost libraries and include dirs.
  SET(BOOST_ROOT ${BOOST_PREFIX})
  SET(Boost_NO_SYSTEM_PATHS ON)

  # Find the boost package
  FIND_PACKAGE(Boost 1.54.0 REQUIRED COMPONENTS ${ARGN})

  # Set the include paths for Boost
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  
  # Echo the Boost details if a verbose configure was requested
  IF(CMAKE_VERBOSE_CONFIGURE)
    MESSAGE("Found BOOST! Here are the details: ")
    MESSAGE(" BOOST_PREFIX = ${BOOST_ROOT}")
    MESSAGE(" BOOST_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}")
    MESSAGE(" BOOST_LIBRARY_DIRS = ${Boost_LIBRARY_DIRS}")
    MESSAGE(" BOOST_LIBRARIES = ${Boost_LIBRARIES}")
    MESSAGE("End of BOOST details\n")
  ENDIF()

ENDMACRO(ENABLE_BOOST_SUPPORT)