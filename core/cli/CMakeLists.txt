ADD_EXECUTABLE(treatmentplanner treatmentplanner.cpp)

TARGET_LINK_LIBRARIES(treatmentplanner ${PROJECT_NAME}_core)

INSTALL(TARGETS treatmentplanner
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)