# Copyright 2011,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

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
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/swig	
    ${CMAKE_BINARY_DIR}/gr-blocks/swig	
    ${CMAKE_BINARY_DIR}/gr-fft/swig	
    ${CMAKE_BINARY_DIR}/gr-filter/swig	
    ${CMAKE_BINARY_DIR}/gr-vocoder/swig	
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
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${CMAKE_CURRENT_SOURCE_DIR}/${py_qa_test_file})
  endforeach(py_qa_test_file)
endif(ENABLE_TESTING)
