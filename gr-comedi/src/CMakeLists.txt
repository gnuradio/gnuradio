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
INCLUDE_DIRECTORIES(
    ${GNURADIO_CORE_INCLUDE_DIRS}
    ${GR_COMEDI_INCLUDE_DIRS}
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${COMEDI_INCLUDE_DIRS})
LINK_DIRECTORIES(${COMEDI_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gr_comedi_sources
    comedi_sink_s.cc
    comedi_source_s.cc
    gri_comedi.cc
)

LIST(APPEND comedi_libs
    gnuradio-core
    ${Boost_LIBRARIES}
    ${COMEDI_LIBRARIES}
)

ADD_LIBRARY(gnuradio-comedi SHARED ${gr_comedi_sources})
TARGET_LINK_LIBRARIES(gnuradio-comedi ${comedi_libs})
SET_TARGET_PROPERTIES(gnuradio-comedi PROPERTIES DEFINE_SYMBOL "gnuradio_comedi_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-comedi PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gnuradio-comedi
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "comedi_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "comedi_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "comedi_runtime" # .dll file
)

########################################################################
# Install public header files
########################################################################
INSTALL(FILES
    comedi_sink_s.h
    comedi_source_s.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "comedi_devel"
)

########################################################################
# Setup swig generation
########################################################################
IF(ENABLE_PYTHON)
INCLUDE(GrPython)
INCLUDE(GrSwig)

SET(GR_SWIG_INCLUDE_DIRS
    ${GR_COMEDI_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

SET(GR_SWIG_LIBRARIES gnuradio-comedi)

GR_SWIG_MAKE(comedi comedi.i)

GR_SWIG_INSTALL(
    TARGETS comedi
    DESTINATION ${GR_PYTHON_DIR}/gnuradio
    COMPONENT "comedi_python"
)

INSTALL(
    FILES comedi.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "comedi_swig"
)

ENDIF(ENABLE_PYTHON)

########################################################################
# Handle the unit tests
########################################################################
IF(ENABLE_TESTING AND ENABLE_PYTHON)
INCLUDE(GrTest)
FILE(GLOB py_qa_test_files "qa_*.py")
FOREACH(py_qa_test_file ${py_qa_test_files})
    GET_FILENAME_COMPONENT(py_qa_test_name ${py_qa_test_file} NAME_WE)
    SET(GR_TEST_PYTHON_DIRS
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/python
        ${CMAKE_BINARY_DIR}/gnuradio-core/src/lib/swig
        ${CMAKE_BINARY_DIR}/gr-comedi/src
    )
    SET(GR_TEST_TARGET_DEPS gruel gnuradio-core gnuradio-comedi)
    GR_ADD_TEST(${py_qa_test_name} ${PYTHON_EXECUTABLE} ${py_qa_test_file})
ENDFOREACH(py_qa_test_file)
ENDIF(ENABLE_TESTING AND ENABLE_PYTHON)
