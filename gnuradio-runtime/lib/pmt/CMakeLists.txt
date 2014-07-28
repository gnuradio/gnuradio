# Copyright 2010-2013 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

########################################################################
# Generate serial tags header file
########################################################################

get_filename_component(PMT_SERIAL_TAGS_H
    ${CMAKE_CURRENT_BINARY_DIR}/../../include/pmt/pmt_serial_tags.h ABSOLUTE
)

add_custom_command(
    OUTPUT ${PMT_SERIAL_TAGS_H}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/gen-serial-tags.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pmt-serial-tags.scm
    COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/gen-serial-tags.py
        ${CMAKE_CURRENT_SOURCE_DIR}/pmt-serial-tags.scm
        ${PMT_SERIAL_TAGS_H}
)

install(
    FILES ${PMT_SERIAL_TAGS_H}
    DESTINATION ${GR_INCLUDE_DIR}/pmt
    COMPONENT "runtime_devel"
)

include(AddFileDependencies)
ADD_FILE_DEPENDENCIES(
    ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc
    ${PMT_SERIAL_TAGS_H}
)

########################################################################
# Generate other pmt stuff
########################################################################
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv_int.h
        ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.h
        ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv.cc
        ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.cc
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/generate_unv.py
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_template.h.t
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_template.cc.t
        ${CMAKE_CURRENT_SOURCE_DIR}/unv_qa_template.cc.t
    COMMAND ${PYTHON_EXECUTABLE} ${PYTHON_DASH_B} -c
    "import os, sys; srcdir='${CMAKE_CURRENT_SOURCE_DIR}'; sys.path.append(srcdir); os.environ['srcdir']=srcdir; from generate_unv import main; main()"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)

set(pmt_sources
  ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_io.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_pool.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc
)

list(APPEND gnuradio_pmt_libs
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

add_custom_target(pmt_generated
  DEPENDS ${PMT_SERIAL_TAGS_H} ${CMAKE_CURRENT_BINARY_DIR}/pmt_unv_int.h)


#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pmt.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc
    @ONLY)

    list(APPEND gr_pmt_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc
    )
endif(MSVC)


add_library(gnuradio-pmt SHARED ${pmt_sources})
target_link_libraries(gnuradio-pmt ${gnuradio_pmt_libs})

GR_LIBRARY_FOO(gnuradio-pmt RUNTIME_COMPONENT "runtime_runtime" DEVEL_COMPONENT "runtime_devel")

add_dependencies(gnuradio-pmt
  pmt_generated
)

if(ENABLE_STATIC_LIBS)
  add_library(gnuradio-pmt_static STATIC ${pmt_sources})

  add_dependencies(gnuradio-pmt_static pmt_generated)

  if(NOT WIN32)
    set_target_properties(gnuradio-pmt_static
      PROPERTIES OUTPUT_NAME gnuradio-pmt)
  endif(NOT WIN32)

  install(TARGETS gnuradio-pmt_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} COMPONENT "runtime_devel"   # .lib file
    )
endif(ENABLE_STATIC_LIBS)

########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)
include(GrTest)

########################################################################
# Append gnuradio-runtime test sources
########################################################################
list(APPEND test_gnuradio_pmt_sources
  qa_pmt.cc
  qa_pmt_prims.cc
  ${CMAKE_CURRENT_BINARY_DIR}/qa_pmt_unv.cc
)

include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

add_library(test-gnuradio-pmt SHARED ${test_gnuradio_pmt_sources})
target_link_libraries(test-gnuradio-pmt gnuradio-runtime gnuradio-pmt
  ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES} ${LOG4CPP_LIBRARIES})

########################################################################
# Build the test executable
# Set the test environment so the build libs will be found under MSVC.
########################################################################
list(APPEND GR_TEST_TARGET_DEPS test-gnuradio-pmt)
add_executable(gr_pmt_test test_pmt.cc)
target_link_libraries(gr_pmt_test test-gnuradio-pmt)
GR_ADD_TEST(gr-pmt-test gr_pmt_test)

endif(ENABLE_TESTING)
