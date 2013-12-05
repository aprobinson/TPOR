# Configure the Moab options that will be required in this project. The 
# following variables will be set:
# 1.) MOAB - stores the Moab library name
MACRO(ENABLE_MOAB_SUPPORT)

  # Use the user supplied prefix to find the MOAB libraries and include dirs.
  SET(MOAB_LIBRARY_DIRS ${MOAB_PREFIX}/lib)
  SET(MOAB_INCLUDE_DIRS ${MOAB_PREFIX}/include)

  # Find the MOAB library
  FIND_LIBRARY(MOAB libMOAB.a ${MOAB_LIBRARY_DIRS})
  
  IF(${MOAB} MATCHES NOTFOUND)
    MESSAGE(FATAL_ERROR "The MOAB library libMOAB.a could not be found.")
  ENDIF()
    
  # Set the MOAB library
  # NOTE: There is a bug in the hdf51.8.12 static library build - the -ldl flag
  #       is not passed to the linker during library creation. It must therefore
  #       be passed here. This should hopefully be fixed in the next version of 
  #       hdf5 that is released.
  SET(MOAB ${MOAB} -lhdf5 -ldl)
  
  # Set the include paths for MOAB
  INCLUDE_DIRECTORIES(${MOAB_INCLUDE_DIRS})

  # Set the link paths for MOAB
  LINK_DIRECTORIES(${MOAB_LIBRARY_DIRS})

  # Echo the Moab details if a verbose configure was requested
  IF(CMAKE_VERBOSE_CONFIGURE)
    MESSAGE("Found MOAB!  Here are the details: ")
    MESSAGE(" MOAB_PREFIX = ${MOAB_PREFIX}")
    MESSAGE(" MOAB_INCLUDE_DIRS = ${MOAB_INCLUDE_DIRS}")
    MESSAGE(" MOAB_LIBRARY_DIRS = ${MOAB_LIBRARY_DIRS}")
    MESSAGE(" MOAB_LIBRARIES = ${MOAB}")
    MESSAGE("End of MOAB details\n")
  ENDIF()

ENDMACRO(ENABLE_MOAB_SUPPORT)