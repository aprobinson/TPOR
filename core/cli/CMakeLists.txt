ADD_EXECUTABLE(treatmentplanner treatmentplanner.cpp)
TARGET_LINK_LIBRARIES(treatmentplanner ${PROJECT_NAME}_core)

ADD_EXECUTABLE(seedmeshgenerator seedmeshgenerator.cpp)
TARGET_LINK_LIBRARIES(seedmeshgenerator ${PROJECT_NAME}_core)

INSTALL(TARGETS treatmentplanner seedmeshgenerator
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)