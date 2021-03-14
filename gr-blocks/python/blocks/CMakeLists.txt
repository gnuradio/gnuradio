# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    parse_file_metadata.py
    stream_to_vector_decimator.py
    msg_pair_to_var.py
    msg_meta_to_pair.py
    var_to_msg.py
    pdu_compatibility.py # REMOVE IN 3.11
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

  set(GR_TEST_TARGET_DEPS "")
  set(GR_TEST_LIBRARY_DIRS "")
  set(GR_TEST_PYTHON_DIRS
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
    )

  include(GrTest)
  file(GLOB py_qa_test_files "qa_*.py")

  # Force out the controlport QA tests if CTRLPORT is not enabled or
  # if it is enabled but we have no backends to use.
  if(NOT ENABLE_GR_CTRLPORT OR CTRLPORT_BACKENDS EQUAL 0)
    list(REMOVE_ITEM py_qa_test_files
      ${CMAKE_CURRENT_SOURCE_DIR}/qa_cpp_py_binding.py
      ${CMAKE_CURRENT_SOURCE_DIR}/qa_cpp_py_binding_set.py
      ${CMAKE_CURRENT_SOURCE_DIR}/qa_ctrlport_probes.py
      )
  endif(NOT ENABLE_GR_CTRLPORT OR CTRLPORT_BACKENDS EQUAL 0)

  if(NOT SNDFILE_FOUND)
    list(REMOVE_ITEM py_qa_test_files
      ${CMAKE_CURRENT_SOURCE_DIR}/qa_wavfile.py
      )
  endif()

  foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
  endforeach(py_qa_test_file)

endif(ENABLE_TESTING)

add_subdirectory(bindings)
