SET(SRC ${CMAKE_SOURCE_DIR}/core/src)

FIND_PACKAGE(Boost REQUIRED)

ADD_EXECUTABLE(tstContractException
  tstContractException.cpp)
ADD_TEST(ContractException_test tstContractException)

ADD_EXECUTABLE(tstBinarySearch
  tstBinarySearch.cpp)
ADD_TEST(BinarySearch_test tstBinarySearch)

ADD_EXECUTABLE(tstInterpolation
  tstInterpolation.cpp
  ${SRC}/Interpolation.cpp)
ADD_TEST(Interpolation_test tstInterpolation)

ADD_EXECUTABLE(tstHDF5FileHandler
  tstHDF5FileHandler.cpp
  ${SRC}/HDF5FileHandler.cpp)
TARGET_LINK_LIBRARIES(tstHDF5FileHandler ${HDF5})
ADD_TEST(HDF5FileHandler_test tstHDF5FileHandler)

ADD_EXECUTABLE(tstBrachytherapySeed
  tstBrachytherapySeed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp)
ADD_TEST(BrachytherapySeed_test tstBrachytherapySeed)

ADD_EXECUTABLE(tstAmersham6702Seed
  tstAmersham6702Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Amersham6702Seed.cpp)
ADD_TEST(Amersham6702Seed_test tstAmersham6702Seed)