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

ADD_EXECUTABLE(tstAmersham6711Seed
  tstAmersham6711Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Amersham6711Seed.cpp)
ADD_TEST(Amersham6711Seed_test tstAmersham6711Seed)

ADD_EXECUTABLE(tstAmersham6733Seed
  tstAmersham6733Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Amersham6733Seed.cpp)
ADD_TEST(Amersham6733Seed_test tstAmersham6733Seed)

ADD_EXECUTABLE(tstBest2301Seed
  tstBest2301Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Best2301Seed.cpp)
ADD_TEST(Best2301Seed_test tstBest2301Seed)

ADD_EXECUTABLE(tstBest2335Seed
  tstBest2335Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Best2335Seed.cpp)
ADD_TEST(Best2335Seed_test tstBest2335Seed)

ADD_EXECUTABLE(tstNASIMED3631Seed
  tstNASIMED3631Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/NASIMED3631Seed.cpp)
ADD_TEST(NASIMED3631Seed_test tstNASIMED3631Seed)

ADD_EXECUTABLE(tstNASIMED3633Seed
  tstNASIMED3633Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/NASIMED3633Seed.cpp)
ADD_TEST(NASIMED3633Seed_test tstNASIMED3633Seed)

ADD_EXECUTABLE(tstBebigI25S06Seed
  tstBebigI25S06Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/BebigI25S06Seed.cpp)
ADD_TEST(BebigI25S06Seed_test tstBebigI25S06Seed)

ADD_EXECUTABLE(tstImagynIS12501Seed
  tstImagynIS12501Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/ImagynIS12501Seed.cpp)
ADD_TEST(ImagynIS12501Seed_test tstImagynIS12501Seed)

ADD_EXECUTABLE(tstTheragenics200Seed
  tstTheragenics200Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/Theragenics200Seed.cpp)
ADD_TEST(Theragenics200Seed_test tstTheragenics200Seed)

ADD_EXECUTABLE(tstDraximageLS1Seed
  tstDraximageLS1Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/DraximageLS1Seed.cpp)
ADD_TEST(DraximageLS1Seed_test tstDraximageLS1Seed)

ADD_EXECUTABLE(tstImplantSciences3500Seed
  tstImplantSciences3500Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/ImplantSciences3500Seed.cpp)
ADD_TEST(ImplantSciences3500Seed_test tstImplantSciences3500Seed)

ADD_EXECUTABLE(tstIBt1251LSeed
  tstIBt1251LSeed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/IBt1251LSeed.cpp)
ADD_TEST(IBt1251LSeed_test tstIBt1251LSeed)

ADD_EXECUTABLE(tstIsoAidIAI125ASeed
  tstIsoAidIAI125ASeed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/IsoAidIAI125ASeed.cpp)
ADD_TEST(IsoAidIAI125ASeed_test tstIsoAidIAI125ASeed)

ADD_EXECUTABLE(tstMBISL125SH125Seed
  tstMBISL125SH125Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/MBISL125SH125Seed.cpp)
ADD_TEST(MBISL125SH125Seed_test tstMBISL125SH125Seed)

ADD_EXECUTABLE(tstSourceTechSTM1251Seed
  tstSourceTechSTM1251Seed.cpp
  ${SRC}/Interpolation.cpp
  ${SRC}/BrachytherapySeed.cpp
  ${SRC}/SourceTechSTM1251Seed.cpp)
ADD_TEST(SourceTechSTM1251Seed_test tstSourceTechSTM1251Seed)
