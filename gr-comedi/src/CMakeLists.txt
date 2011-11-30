# Copyright 2011 Free Software Foundation, Inc.
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
# Setup the include and linker paths
########################################################################
include_directories(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_COMEDI_INCLUDE_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${COMEDI_INCLUDE_DIRS})
link_directories(${COMEDI_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_comedi_sources
    comedi_sink_s.cc
    comedi_source_s.cc
    gri_comedi.cc
)

list(APPEND comedi_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    ${COMEDI_LIBRARIES}
)

add_library(gnuradio-comedi SHARED ${gr_comedi_sources})
target_link_libraries(gnuradio-comedi ${comedi_libs})
GR_LIBRARY_FOO(gnuradio-comedi RUNTIME_COMPONENT "comedi_runtime" DEVEL_COMPONENT "comedi_devel")

########################################################################
# Install public header files
########################################################################
install(FILES
    comedi_sink_s.h
    comedi_source_s.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "comedi_devel"
)

########################################################################
# Setup swig generation
########################################################################
if(ENABLE_PYTHON)
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_COMEDI_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/comedi_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR})

set(GR_SWIG_LIBRARIES gnuradio-comedi)

GR_SWIG_MAKE(comedi comedi.i)

GR_SWIG_INSTALL(
    TARGETS comedi
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "comedi_python"
)

install(
    FILES comedi.i
    ${CMAKE_CURRENT_BINARY_DIR}/comedi_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "comedi_swig"
)

endif(ENABLE_PYTHON)

########################################################################
# Handle the unit tests
########################################################################
if(ENABLE_TESTING AND ENABLE_PYTHON)
include(GrTest)
file(GLOB py_qa_test_files "qa_*.py")
foreach(py_qa_test_file ${py_qa_test_files})
    get_filename_component(py_qa_test_name ${py_qa_test_file} NAME_WE)
    set(GR_TEST_PYTHON_DIRS
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/python
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig
        ${CMAKE_BINARY_DIR}/gr-comedi/src
    )
    set(GR_TEST_TARGET_DEPS gruel gnuradio-core gnuradio-comedi)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${py_qa_test_file})
endforeach(py_qa_test_file)
endif(ENABLE_TESTING AND ENABLE_PYTHON)
