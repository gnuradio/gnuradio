# Copyright 2019 Free Software Foundation, Inc.
# Copyright 2024 Marcus Müller
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

  set(GR_TEST_TARGET_DEPS "")
  set(GR_TEST_LIBRARY_DIRS "")
  set(GR_TEST_PYTHON_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/../..
    ${PROJECT_BINARY_DIR}/gnuradio-runtime/python
    )

  include(GrTest)
  file(GLOB py_qa_test_files "test_*.py")
  file(GLOB shell_qa_test_files "test_*.sh")

  foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    if (CMAKE_CROSSCOMPILING)
        get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
    endif(CMAKE_CROSSCOMPILING)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
  endforeach(py_qa_test_file)

  find_program (BASH_EXECUTABLE bash)
  if(BASH_EXECUTABLE)
    foreach(shell_qa_test_file ${shell_qa_test_files})
      get_filename_component(shell_qa_test_name ${shell_qa_test_file} NAME_WE)
      if (CMAKE_CROSSCOMPILING)
        get_filename_component(shell_qa_test_file ${shell_qa_test_file} NAME)
      endif(CMAKE_CROSSCOMPILING)
      GR_ADD_TEST(${shell_qa_test_name} ${BASH_EXECUTABLE} ${shell_qa_test_file})
    endforeach(shell_qa_test_file)
  endif(BASH_EXECUTABLE)

endif(ENABLE_TESTING)
