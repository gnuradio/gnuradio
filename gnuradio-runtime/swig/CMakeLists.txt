# Copyright 2013 Free Software Foundation, Inc.
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
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_SWIG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Boost_INCLUDE_DIRS}
)

link_directories(${Boost_LIBRARY_DIRS})

set(GR_SWIG_LIBRARIES
    gnuradio-pmt
    gnuradio-runtime
    ${LOG4CPP_LIBRARIES}
)

###############################################################
#    Build pmt swig
###############################################################

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/pmt
  ${CMAKE_CURRENT_BINARY_DIR}/../include/pmt)
list(APPEND GR_SWIG_TARGET_DEPS pmt_generated)
GR_SWIG_MAKE(pmt_swig pmt_swig.i)

GR_SWIG_INSTALL(
    TARGETS pmt_swig
    DESTINATION ${GR_PYTHON_DIR}/pmt
    COMPONENT "runtime_python"
)

add_custom_target(pmt_swig DEPENDS ${SWIG_MODULE_pmt_swig_REAL_NAME})

###############################################################
#    Build gnuradio-runtime swig
###############################################################

# Only use if log4cpp is installed
# Define ENABLE_GR_LOG for the .i file to ignore it.
if(ENABLE_GR_LOG)
     SET(GR_SWIG_FLAGS "-DENABLE_GR_LOG")
endif(ENABLE_GR_LOG)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
  list(APPEND GR_SWIG_FLAGS -DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(GR_RUNTIME_SWIG_DOC_FILE ${GR_SWIG_DOC_FILE})
list(APPEND GR_SWIG_TARGET_DEPS pmt_swig)

GR_SWIG_MAKE(runtime_swig runtime_swig.i)

GR_SWIG_INSTALL(
    TARGETS runtime_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr
    COMPONENT "runtime_python"
)

install(
    FILES
    gnuradio_swig_bug_workaround.h
    runtime_swig.i
    gnuradio.i
    basic_block.i
    block.i
    block_detail.i
    block_gateway.i
    buffer.i
    complex_vec_test.i
    constants.i
    feval.i
    gnuradio.i
    gr_ctrlport.i
    gr_extras.i
    gr_intrusive_ptr.i
    gr_logger.i
    gr_shared_ptr.i
    gr_swig_block_magic.i
    gr_types.i
    hier_block2.i
    io_signature.i
    message.i
    msg_handler.i
    msg_queue.i
    pmt_swig.i
    prefs.i
    realtime.i
    single_threaded_scheduler.i
    sync_block.i
    sync_decimator.i
    sync_interpolator.i
    tagged_stream_block.i
    tags.i
    top_block.i
    ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i
    ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "runtime_swig"
)
