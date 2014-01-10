# Define the minimum required version of CMake
CMAKE_MINIMUM_REQUIRED(VERSION 2.8 FATAL_ERROR)

# Add the custom modules to the module path
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
  "${CMAKE_DIR}/share/cmake-2.8/Modules"
  ${PROJECT_SOURCE_DIR}/cmake)

# Define the project name
INCLUDE(ProjectName)

# Define the project version
INCLUDE(Version)

# Include the macros that have been defined for this project
INCLUDE(Macros)

# Enforce an out-of-source build
ENFORCE_OUT_OF_SOURCE_BUILD()

# Enable MOAB Support
IF(MOAB_PREFIX)
  ENABLE_MOAB_SUPPORT()
ELSE()
  MESSAGE(FATAL_ERROR "The MOAB_PREFIX must be set!")
ENDIF()

# Enable HDF5 Support
IF(HDF5_PREFIX)
  ENABLE_HDF5_SUPPORT(libhdf5_cpp.a)
ELSE()
  MESSAGE(FATAL_ERROR "The HDF5_PREFIX must be set!")
ENDIF()

# Enable BOOST Support
IF(BOOST_PREFIX)
  ENABLE_BOOST_SUPPORT(program_options test_exec_monitor chrono system)
ELSE()
  MESSAGE(STATUS "The BOOST_PREFIX has not been set. The system default will be used.")
ENDIF()

# Add Design-by-Contract support if requested
IF(${${PROJECT_NAME}_ENABLE_DBC})
  SET(HAVE_${PROJECT_NAME}_DBC "1")
ELSE()
  SET(HAVE_${PROJECT_NAME}_DBC "0")
ENDIF()

# Parse the DBC configure file so it can be used in the source files
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/cmake/dbc_config.hpp.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}_config.hpp)

# Add the CMAKE_BINARY_DIR to the include path to access config files
INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

# Add an uninstall target
ADD_UNINSTALL_TARGET()

# Enable testing through CTest
ENABLE_TESTING()
INCLUDE(Dart)

# Set up the project directory hierarchy
ADD_SUBDIRECTORY(core)

ADD_SUBDIRECTORY(pytpor)

ADD_SUBDIRECTORY(scripts)