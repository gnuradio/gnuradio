# Copyright 2010-2012,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

set(PY_QT_IMPORT "from PyQt5 import Qt, QtCore, QtWidgets")

configure_file(range.py.cmakein "${CMAKE_CURRENT_BINARY_DIR}/range.py" @ONLY)
configure_file(util.py.cmakein "${CMAKE_CURRENT_BINARY_DIR}/util.py" @ONLY)

GR_PYTHON_INSTALL(
    FILES __init__.py
          "${CMAKE_CURRENT_BINARY_DIR}/range.py"
          "${CMAKE_CURRENT_BINARY_DIR}/util.py"
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/qtgui
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

  set(GR_TEST_TARGET_DEPS "")
  set(GR_TEST_LIBRARY_DIRS "")
  set(GR_TEST_PYTHON_DIRS
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/swig	
    ${CMAKE_BINARY_DIR}/gr-qtgui/swig
    )

  include(GrTest)
  file(GLOB py_qa_test_files "qa_*.py")
  foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
  endforeach(py_qa_test_file)
endif(ENABLE_TESTING)
