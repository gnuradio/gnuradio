# Copyright 2011,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Setup python install
########################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    cvsd.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/vocoder
)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING)

  set(GR_TEST_TARGET_DEPS "")
  set(GR_TEST_LIBRARY_DIRS "")
  set(GR_TEST_PYTHON_DIRS
    ${PROJECT_BINARY_DIR}/gnuradio-runtime/python
    )

  include(GrTest)
  list(APPEND py_qa_test_files
    qa_alaw_vocoder.py
    qa_cvsd_vocoder.py
    qa_g721_vocoder.py
    qa_g723_24_vocoder.py
    qa_g723_40_vocoder.py
    qa_ulaw_vocoder.py
    )
  if(LIBCODEC2_FOUND)
    list(APPEND py_qa_test_files
      qa_codec2_vocoder.py
      )
  endif()
  if(LIBGSM_FOUND)
    list(APPEND py_qa_test_files
      qa_gsm_full_rate.py
      )
  endif()
  foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    if (CMAKE_CROSSCOMPILING)
        get_filename_component(py_qa_test_file ${py_qa_test_file} NAME)
    endif(CMAKE_CROSSCOMPILING)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${CMAKE_CURRENT_SOURCE_DIR}/${py_qa_test_file})
  endforeach(py_qa_test_file)
endif(ENABLE_TESTING)

add_subdirectory(bindings)
