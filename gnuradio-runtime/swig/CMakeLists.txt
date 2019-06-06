# Copyright 2013,2019 Free Software Foundation, Inc.
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/gnuradio/swig>
  )

set(GR_SWIG_LIBRARIES
    gnuradio-runtime)

###############################################################
#    Build pmt swig
###############################################################

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/pmt
  ${CMAKE_CURRENT_BINARY_DIR}/../include/pmt)
GR_SWIG_MAKE(pmt_swig pmt_swig.i)

GR_SWIG_INSTALL(
    TARGETS pmt_swig
    DESTINATION ${GR_PYTHON_DIR}/pmt
)

###############################################################
#    Build gnuradio-runtime swig
###############################################################
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(GR_RUNTIME_SWIG_DOC_FILE ${GR_SWIG_DOC_FILE})
if(ENABLE_GR_CTRLPORT)
  set(GR_SWIG_FLAGS "-DGR_CTRLPORT")
endif()
list(APPEND GR_SWIG_TARGET_DEPS pmt_swig)

GR_SWIG_MAKE(runtime_swig runtime_swig.i)

install(
  TARGETS runtime_swig
  EXPORT runtime_swig-export
  DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr
  )

include(GrPython)
GR_PYTHON_INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig.py
  DESTINATION ${GR_PYTHON_DIR}/gnuradio/gr
  DEPENDS runtime_swig
)

install(EXPORT runtime_swig-export
  FILE runtime_swigTargets.cmake
  NAMESPACE gnuradio::
  DESTINATION ${GR_CMAKE_DIR}
)

install(
    FILES
    runtime_swig.i
    gnuradio.i
    basic_block.i
    block.i
    block_detail.i
    block_gateway.i
    buffer.i
    constants.i
    feval.i
    gr_ctrlport.i
    gr_extras.i
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
    py3compat.i
    realtime.i
    sync_block.i
    sync_decimator.i
    sync_interpolator.i
    tagged_stream_block.i
    tags.i
    top_block.i
    ${CMAKE_CURRENT_BINARY_DIR}/runtime_swig_doc.i
    ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
)
