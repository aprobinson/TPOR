SET(SRC ${CMAKE_SOURCE_DIR}/src)

ADD_EXECUTABLE(tstContractException
  tstContractException.cpp
  ${TEUCHOS_STD_UNIT_TEST_MAIN})

TARGET_LINK_LIBRARIES(tstContractException ${TEUCHOS_CORE})

ADD_TEST(ContractException_test tstContractException)