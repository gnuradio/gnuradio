# Copyright 2021 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Include python install macros
########################################################################
include(GrPython)

########################################################################
# Install python sources
########################################################################
GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/iio
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
	file(GLOB py_qa_test_files "qa_*.py")
	foreach(py_qa_test_file ${py_qa_test_files})
	    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
	    GR_ADD_TEST(${py_qa_test_name} ${QA_PYTHON_EXECUTABLE} -B ${py_qa_test_file})
	endforeach(py_qa_test_file)
endif(ENABLE_TESTING)

add_subdirectory(bindings)
