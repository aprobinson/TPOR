# Configure the HDF5 options that will be required in this project. The 
# following variables will be set:
# 1.) HDF5 - stores the HDF5 cpp wrapper library and the HDF5 core library
MACRO(ENABLE_HDF5_SUPPORT)

  # Use the user supplied prefix to find the HDF5 libraries and include dirs.
  SET(HDF5_INCLUDE_DIRS 
    ${HDF5_PREFIX}/include
    ${HDF5_PREFIX}/include/cpp
    ${HDF5_PREFIX}/include/hl)
  SET(HDF5_LIBRARY_DIRS ${HDF5_PREFIX}/lib)
 
  # Find the libraries requested by the user
  FOREACH(HDF5LIB_NAME ${ARGN})
    FIND_LIBRARY(HDF5LIB ${HDF5LIB_NAME} ${HDF5_LIBRARY_DIRS})
    
    IF(${HDF5LIB} MATCHES NOTFOUND)
      MESSAGE(FATAL_ERROR "The HDF5 library ${HDF5LIB} could not be found.")
    ENDIF()
    
    SET(HDF5 ${HDF5LIB} ${HDF5})
  ENDFOREACH()
  
  # Set the include paths for HDF5
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIRS})
  
  # Set the link paths for HDF5
  LINK_DIRECTORIES(${HDF5_LIBRARY_DIRS})

  # Echo the HDF5 details if a verbose configure was requested
  IF(CMAKE_VERBOSE_CONFIGURE)
    MESSAGE("Found HDF5! Here are the details: ")
    MESSAGE(" HDF5_PREFIX = ${HDF5_PREFIX}")
    MESSAGE(" HDF5_INCLUDE_DIRS = ${HDF5_INCLUDE_DIRS}")
    MESSAGE(" HDF5_LIBRARY_DIRS = ${HDF5_LIBRARY_DIRS}")
    MESSAGE(" HDF5_LIBRARY = ${HDF5}")
    MESSAGE("End of HDF5 details\n")
  ENDIF()

ENDMACRO(ENABLE_HDF5_SUPPORT)