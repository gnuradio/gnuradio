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
    ${GRUEL_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_SWIG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${LOG4CPP_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

set(GR_SWIG_LIBRARIES
    gnuradio-runtime
    ${LOG4CPP_LIBRARIES}
)

# Only use if log4cpp is installed
# Define ENABLE_GR_LOG for the .i file to ignore it.
if(ENABLE_GR_LOG)
     SET(GR_SWIG_FLAGS "-DENABLE_GR_LOG")
endif(ENABLE_GR_LOG)

if(ENABLE_GR_CTRLPORT)
  list(APPEND GR_SWIG_FLAGS -DGR_CTRLPORT)
  list(APPEND GR_SWIG_LIBRARIES ${ICE_LIBRARIES})
  list(APPEND GR_SWIG_INCLUDE_DIRS ${ICE_INCLUDE_DIR})
endif(ENABLE_GR_CTRLPORT)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

link_directories(${Boost_LIBRARY_DIRS})

set(GR_SWIG_TARGET_DEPS 
  pmt_swig
  ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i
)

GR_SWIG_MAKE(runtime_swig runtime_swig.i)

GR_SWIG_INSTALL(
    TARGETS runtime_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr
    COMPONENT "runtime_python"
)

install(
    FILES
    gnuradio_swig_bug_workaround.h
    complex_vec_test.i
    gnuradio.i
    gr_basic_block.i
    gr_block_detail.i
    gr_block.i
    gr_buffer.i
    gr_constants.i
    gr_dispatcher.i
    gr_error_handler.i
    gr_feval.i
    gr_hier_block2.i
    gr_io_signature.i
    gr_logger.i
    gr_message.i
    gr_msg_handler.i
    gr_msg_queue.i
    gr_prefs.i
    gr_realtime.i
    gr_shared_ptr.i
    gr_single_threaded_scheduler.i
    gr_swig_block_magic.i
    gr_sync_block.i
    gr_sync_decimator.i
    gr_sync_interpolator.i
    gr_tagged_stream_block.i
    gr_tags.i
    gr_top_block.i
    runtime_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "runtime_swig"
)
