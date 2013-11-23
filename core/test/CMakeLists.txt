ADD_EXECUTABLE(tstContractException
  tstContractException.cpp)
TARGET_LINK_LIBRARIES(tstContractException ${PROJECT_NAME}_core)
ADD_TEST(ContractException_test tstContractException)

ADD_EXECUTABLE(tstBinarySearch
  tstBinarySearch.cpp)
TARGET_LINK_LIBRARIES(tstBinarySearch ${PROJECT_NAME}_core)
ADD_TEST(BinarySearch_test tstBinarySearch)

ADD_EXECUTABLE(tstInterpolation
  tstInterpolation.cpp)
TARGET_LINK_LIBRARIES(tstInterpolation ${PROJECT_NAME}_core)
ADD_TEST(Interpolation_test tstInterpolation)

ADD_EXECUTABLE(tstHDF5FileHandler
  tstHDF5FileHandler.cpp)
TARGET_LINK_LIBRARIES(tstHDF5FileHandler ${PROJECT_NAME}_core)
ADD_TEST(HDF5FileHandler_test tstHDF5FileHandler)

ADD_EXECUTABLE(tstBrachytherapyPatientFileHandler
  tstBrachytherapyPatientFileHandler.cpp)
TARGET_LINK_LIBRARIES(tstBrachytherapyPatientFileHandler ${PROJECT_NAME}_core)
ADD_TEST(BrachytherapyPatientFileHandler_test 
  tstBrachytherapyPatientFileHandler)

ADD_EXECUTABLE(tstBrachytherapySeed
  tstBrachytherapySeed.cpp)
TARGET_LINK_LIBRARIES(tstBrachytherapySeed ${PROJECT_NAME}_core)
ADD_TEST(BrachytherapySeed_test tstBrachytherapySeed)

ADD_EXECUTABLE(tstAmersham6702Seed
  tstAmersham6702Seed.cpp)
TARGET_LINK_LIBRARIES(tstAmersham6702Seed ${PROJECT_NAME}_core)
ADD_TEST(Amersham6702Seed_test tstAmersham6702Seed)

ADD_EXECUTABLE(tstAmersham6711Seed
  tstAmersham6711Seed.cpp)
TARGET_LINK_LIBRARIES(tstAmersham6711Seed ${PROJECT_NAME}_core)
ADD_TEST(Amersham6711Seed_test tstAmersham6711Seed)

ADD_EXECUTABLE(tstAmersham6733Seed
  tstAmersham6733Seed.cpp)
TARGET_LINK_LIBRARIES(tstAmersham6733Seed ${PROJECT_NAME}_core)
ADD_TEST(Amersham6733Seed_test tstAmersham6733Seed)

ADD_EXECUTABLE(tstAmersham9011Seed
  tstAmersham9011Seed.cpp)
TARGET_LINK_LIBRARIES(tstAmersham9011Seed ${PROJECT_NAME}_core)
ADD_TEST(Amersham9011Seed_test tstAmersham9011Seed)

ADD_EXECUTABLE(tstBest2301Seed
  tstBest2301Seed.cpp)
TARGET_LINK_LIBRARIES(tstBest2301Seed ${PROJECT_NAME}_core)
ADD_TEST(Best2301Seed_test tstBest2301Seed)

ADD_EXECUTABLE(tstBest2335Seed
  tstBest2335Seed.cpp)
TARGET_LINK_LIBRARIES(tstBest2335Seed ${PROJECT_NAME}_core)
ADD_TEST(Best2335Seed_test tstBest2335Seed)

ADD_EXECUTABLE(tstNASIMED3631Seed
  tstNASIMED3631Seed.cpp)
TARGET_LINK_LIBRARIES(tstNASIMED3631Seed ${PROJECT_NAME}_core)
ADD_TEST(NASIMED3631Seed_test tstNASIMED3631Seed)

ADD_EXECUTABLE(tstNASIMED3633Seed
  tstNASIMED3633Seed.cpp)
TARGET_LINK_LIBRARIES(tstNASIMED3633Seed ${PROJECT_NAME}_core)
ADD_TEST(NASIMED3633Seed_test tstNASIMED3633Seed)

ADD_EXECUTABLE(tstBebigI25S06Seed
  tstBebigI25S06Seed.cpp)
TARGET_LINK_LIBRARIES(tstBebigI25S06Seed ${PROJECT_NAME}_core)
ADD_TEST(BebigI25S06Seed_test tstBebigI25S06Seed)

ADD_EXECUTABLE(tstImagynIS12501Seed
  tstImagynIS12501Seed.cpp)
TARGET_LINK_LIBRARIES(tstImagynIS12501Seed ${PROJECT_NAME}_core)
ADD_TEST(ImagynIS12501Seed_test tstImagynIS12501Seed)

ADD_EXECUTABLE(tstTheragenics200Seed
  tstTheragenics200Seed.cpp)
TARGET_LINK_LIBRARIES(tstTheragenics200Seed ${PROJECT_NAME}_core)
ADD_TEST(Theragenics200Seed_test tstTheragenics200Seed)

ADD_EXECUTABLE(tstTheragenicsAgX100Seed
  tstTheragenicsAgX100Seed.cpp)
TARGET_LINK_LIBRARIES(tstTheragenicsAgX100Seed ${PROJECT_NAME}_core)
ADD_TEST(TheragenicsAgX100Seed_test tstTheragenicsAgX100Seed)

ADD_EXECUTABLE(tstDraximageLS1Seed
  tstDraximageLS1Seed.cpp)
TARGET_LINK_LIBRARIES(tstDraximageLS1Seed ${PROJECT_NAME}_core)
ADD_TEST(DraximageLS1Seed_test tstDraximageLS1Seed)

ADD_EXECUTABLE(tstImplantSciences3500Seed
  tstImplantSciences3500Seed.cpp)
TARGET_LINK_LIBRARIES(tstImplantSciences3500Seed ${PROJECT_NAME}_core)
ADD_TEST(ImplantSciences3500Seed_test tstImplantSciences3500Seed)

ADD_EXECUTABLE(tstIBt1251LSeed
  tstIBt1251LSeed.cpp)
TARGET_LINK_LIBRARIES(tstIBt1251LSeed ${PROJECT_NAME}_core)
ADD_TEST(IBt1251LSeed_test tstIBt1251LSeed)

ADD_EXECUTABLE(tstIsoAidIAI125ASeed
  tstIsoAidIAI125ASeed.cpp)
TARGET_LINK_LIBRARIES(tstIsoAidIAI125ASeed ${PROJECT_NAME}_core)
ADD_TEST(IsoAidIAI125ASeed_test tstIsoAidIAI125ASeed)

ADD_EXECUTABLE(tstIsoAidIAPd103ASeed
  tstIsoAidIAPd103ASeed.cpp)
TARGET_LINK_LIBRARIES(tstIsoAidIAPd103ASeed ${PROJECT_NAME}_core)
ADD_TEST(IsoAidIAPd103ASeed_test tstIsoAidIAPd103ASeed)

ADD_EXECUTABLE(tstMBISL125SH125Seed
  tstMBISL125SH125Seed.cpp)
TARGET_LINK_LIBRARIES(tstMBISL125SH125Seed ${PROJECT_NAME}_core)
ADD_TEST(MBISL125SH125Seed_test tstMBISL125SH125Seed)

ADD_EXECUTABLE(tstSourceTechSTM1251Seed
  tstSourceTechSTM1251Seed.cpp)
TARGET_LINK_LIBRARIES(tstSourceTechSTM1251Seed ${PROJECT_NAME}_core)
ADD_TEST(SourceTechSTM1251Seed_test tstSourceTechSTM1251Seed)

ADD_EXECUTABLE(tstNucletron130002Seed
  tstNucletron130002Seed.cpp)
TARGET_LINK_LIBRARIES(tstNucletron130002Seed ${PROJECT_NAME}_core)
ADD_TEST(Nucletron130002Seed_test tstNucletron130002Seed)

ADD_EXECUTABLE(tstBrachytherapySeedFactory
  tstBrachytherapySeedFactory.cpp)
TARGET_LINK_LIBRARIES(tstBrachytherapySeedFactory ${PROJECT_NAME}_core)
ADD_TEST(BrachtherapySeedFactory_test tstBrachytherapySeedFactory)

ADD_EXECUTABLE(tstBrachytherapyCommandLineProcessor
  tstBrachytherapyCommandLineProcessor.cpp)
TARGET_LINK_LIBRARIES(tstBrachytherapyCommandLineProcessor ${PROJECT_NAME}_core)
ADD_TEST(BrachytherapyCommandLineProcessor_test 
  tstBrachytherapyCommandLineProcessor test_file.h5 --treatment_planner=IIEMTreatmentPlanner --seeds=Amersham6711Seed --prescribed_dose=145)

ADD_EXECUTABLE(tstIIEMTreatmentPlanner
  tstIIEMTreatmentPlanner.cpp)
TARGET_LINK_LIBRARIES(tstIIEMTreatmentPlanner ${PROJECT_NAME}_core)
ADD_TEST(IIEMTreatmentPlanner_test tstIIEMTreatmentPlanner)