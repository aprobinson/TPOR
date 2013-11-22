# Add a python target that will be compiled to a .pyo file
# Make sure that FIND_PACKAGE(PythonInterp) is called before this macro
MACRO(ADD_PYTHON_TARGET PYTHON_TARGET PYTHON_FILE)
  
  # Compile the target python file
  SET(OUT ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_TARGET})
  LIST(APPEND OUT_FILES ${OUT})
  ADD_CUSTOM_COMMAND(OUTPUT ${OUT}
    COMMAND ${PYTHON_EXECUTABLE} -O -c 'import py_compile\; py_compile.compile(\"${CMAKE_CURRENT_SOURCE_DIR}/${PYTHON_FILE}\", \"${PYTHON_TARGET}\")')

  # Compile every file that the target file depends on
  # FOREACH(PYFILE ${ARGN})
  #   SET(OUT ${CMAKE_CURRENT_BINARY_DIR}/${PYFILE}.pyo)
  #   LIST(APPEND OUT_FILES ${OUT})
  #   ADD_CUSTOM_COMMAND(OUTPUT ${OUT}
  #     COMMAND ${PYTHON_EXECUTABLE} -O -m py_compile ${CMAKE_CURRENT_SOURCE_DIR}/${PYFILE})
  # ENDFOREACH()
  
  # Add the python target to the Makefile
  # ADD_CUSTOM_TARGET(${PYTHON_TARGET} ALL
  #   DEPENDS ${OUT_FILES})
  ADD_CUSTOM_TARGET(${PYTHON_TARGET} ALL
    DEPENDS ${PYTHON_FILE})
  
ENDMACRO()