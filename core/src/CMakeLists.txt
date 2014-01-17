# Find all headers and sources
FILE(GLOB HEADERS *.hpp)
FILE(GLOB SOURCES *.cpp)

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

# ADD_LIBRARY(${PROJECT_NAME}_seeds SHARED ${SEED_SOURCES})
# SET_TARGET_PROPERTIES(${PROJECT_NAME}_seeds
#   PROPERTIES
#   INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)
# TARGET_LINK_LIBRARIES(${PROJECT_NAME}_seeds ${HDF5} ${MOAB})

ADD_LIBRARY(${PROJECT_NAME}_seeds STATIC ${SEED_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_seeds ${MOAB} ${HDF5})

ADD_LIBRARY(${PROJECT_NAME}_core STATIC ${SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}_core ${MOAB} ${HDF5} ${Boost_LIBRARIES})
  
INSTALL(TARGETS 
  ${PROJECT_NAME}_core ${PROJECT_NAME}_seeds
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

INSTALL(FILES ${HEADERS}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
