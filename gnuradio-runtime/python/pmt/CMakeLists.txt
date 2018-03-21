# Copyright 2011,2013 Free Software Foundation, Inc.
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

include(GrPython)

########################################################################
# Install python files
########################################################################
GR_PYTHON_INSTALL(FILES
    __init__.py
    pmt_to_python.py
    DESTINATION ${GR_PYTHON_DIR}/pmt
    COMPONENT "runtime_python"
)

########################################################################
# Setup unit tests
########################################################################
if(ENABLE_TESTING)
include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")
foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    set(GR_TEST_PYTHON_DIRS
        ${CMAKE_BINARY_DIR}/gnuradio-runtime/python
        ${CMAKE_BINARY_DIR}/gnuradio-runtime/swig
        ${CMAKE_BINARY_DIR}/gnuradio-blocks/swig		
    )
    set(GR_TEST_TARGET_DEPS gnuradio-runtime)
    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} ${PYTHON_DASH_B} ${py_qa_test_file})
endforeach(py_qa_test_file)
endif(ENABLE_TESTING)
