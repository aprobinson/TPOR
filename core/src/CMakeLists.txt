SET(HEADERS ${HEADERS}
  Amersham6702Seed.hpp
  Amersham6711Seed.hpp
  Amersham6733Seed.hpp
  Amersham9011Seed.hpp
  ArrayTraitsDecl.hpp
  ArrayTraits.hpp
  BebigI25S06Seed.hpp
  Best2301Seed.hpp
  Best2335Seed.hpp
  BinarySearch_def.hpp
  BinarySearch.hpp
  BrachytherapyAdjointDataGenerator.hpp
  BrachytherapyCommandLineProcessor.hpp
  BrachytherapyPatientFileHandler.hpp
  BrachytherapySeedFactory.hpp
  BrachytherapySeedFileHandler.hpp
  BrachytherapySeedHelpers.hpp
  BrachytherapySeed.hpp
  BrachytherapySeedPosition.hpp
  BrachytherapySeedPosition_def.hpp
  BrachytherapySeedProxy.hpp
  BrachytherapySeedType.hpp
  BrachytherapyTreatmentPlannerFactory.hpp
  BrachytherapyTreatmentPlannerType.hpp
  ContractException.hpp
  DraximageLS1Seed.hpp
  Empty.hpp
  ExceptionCatchMacros.hpp
  ExceptionTestMacros.hpp
  HDF5FileHandler_def.hpp
  HDF5FileHandler.hpp
  HDF5TypeTraitsDecl.hpp
  HDF5TypeTraits.hpp
  IBt1251LSeed.hpp
  IIEMTreatmentPlanner.hpp
  ImagynIS12501Seed.hpp
  ImplantSciences3500Seed.hpp
  Interpolation.hpp
  IsoAidIAI125ASeed.hpp
  IsoAidIAPd103ASeed.hpp
  MBISL125SH125Seed.hpp
  MOABException.hpp
  NASIMED3631Seed.hpp
  NASIMED3633Seed.hpp
  Nucletron130002Seed.hpp
  SourceTechSTM1251Seed.hpp
  Theragenics200Seed.hpp
  TheragenicsAgX100Seed.hpp
  UndefinedTraits.hpp)

SET(SEED_SOURCES ${SEED_SOURCES}
  Amersham6702Seed.cpp
  Amersham6711Seed.cpp
  Amersham6733Seed.cpp
  Amersham9011Seed.cpp
  BebigI25S06Seed.cpp
  Best2301Seed.cpp
  Best2335Seed.cpp
  BrachytherapySeed.cpp
  BrachytherapySeedFactory.cpp
  BrachytherapySeedHelpers.cpp
  DraximageLS1Seed.cpp
  IBt1251LSeed.cpp
  ImagynIS12501Seed.cpp
  ImplantSciences3500Seed.cpp
  IsoAidIAI125ASeed.cpp
  IsoAidIAPd103ASeed.cpp
  MBISL125SH125Seed.cpp
  NASIMED3631Seed.cpp
  NASIMED3633Seed.cpp
  Nucletron130002Seed.cpp
  SourceTechSTM1251Seed.cpp
  Theragenics200Seed.cpp
  TheragenicsAgX100Seed.cpp)

SET(SOURCES ${SOURCES} ${SEED_SOURCES}
  BrachytherapyAdjointDataGenerator.cpp
  BrachytherapyCommandLineProcessor.cpp
  BrachytherapyPatientFileHandler.cpp
  BrachytherapySeedFileHandler.cpp
  BrachytherapySeedPosition.cpp
  BrachytherapySeedProxy.cpp
  BrachytherapyTreatmentPlannerFactory.cpp
  HDF5FileHandler.cpp
  IIEMTreatmentPlanner.cpp
  Interpolation.cpp)

# ADD_LIBRARY(${PROJECT_NAME}_seeds SHARED ${SEED_SOURCES})
# SET_TARGET_PROPERTIES(${PROJECT_NAME}_seeds
#   PROPERTIES
#   INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME}_seeds ${HDF5} ${MOAB})

ADD_LIBRARY(${PROJECT_NAME}_seeds STATIC ${SEED_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_seeds ${HDF5} ${MOAB})

ADD_LIBRARY(${PROJECT_NAME}_core STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_core ${HDF5} ${MOAB} ${Boost_LIBRARIES})
  
INSTALL(TARGETS 
  ${PROJECT_NAME}_core ${PROJECT_NAME}_seeds
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
