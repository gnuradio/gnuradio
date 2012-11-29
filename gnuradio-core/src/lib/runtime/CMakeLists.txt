# Copyright 2010-2010 Free Software Foundation, Inc.
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

########################################################################
# Control availability of vmcircbuf methods.
# For now, only allows disabling of shm methods, which cause uncatchable
#    segmentation faults on Cygwin with gcc 4.x (x <= 5)
# Usage:
#   GR_VMCIRCBUF()
#
# Will set TRY_SHM_VMCIRCBUF to 1 by default except on Windows machines.
# Can manually set with -DTRY_SHM_VMCIRCBUF=0|1
########################################################################

  if(WIN32)
    OPTION(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" OFF)
  else(WIN32)
    OPTION(TRY_SHM_VMCIRCBUF "Try SHM VMCIRCBUF" ON)
  endif(WIN32)

  message(STATUS "TRY_SHM_VMCIRCBUF set to ${TRY_SHM_VMCIRCBUF}.")

  if(TRY_SHM_VMCIRCBUF)
    add_definitions( -DTRY_SHM_VMCIRCBUF )
  endif(TRY_SHM_VMCIRCBUF)

########################################################################
# Append gnuradio-core library sources
########################################################################
list(APPEND gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_basic_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_flowgraph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_flat_flowgraph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block_detail.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block_executor.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2_detail.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_buffer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_dispatcher.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_error_handler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_io_signature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_local_sighandler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_message.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_accepter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_handler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_queue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_pagesize.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_preferences.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_realtime.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler_sts.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler_tpb.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_single_threaded_scheduler.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sptr_magic.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_decimator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_interpolator.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sys_paths.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block_impl.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tpb_detail.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tpb_thread_body.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf_mmap_shm_open.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf_mmap_tmpfile.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf_createfilemapping.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf_sysv_shm.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_select_handler.cc
)

########################################################################
# Append gnuradio-core test sources
########################################################################
list(APPEND test_gnuradio_core_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hier_block2.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_hier_block2_derived.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_buffer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_flowgraph.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_top_block.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_io_signature.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_gr_vmcircbuf.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_block_tags.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_runtime.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_set_msg_handler.cc
)

########################################################################
# Install runtime headers
########################################################################
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_basic_block.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_flowgraph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_flat_flowgraph.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block_detail.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block_executor.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2_detail.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_buffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_complex.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_dispatcher.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_error_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_io_signature.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_local_sighandler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_accepter.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_pagesize.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_preferences.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_realtime.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_runtime_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler_sts.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_scheduler_tpb.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_select_handler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_single_threaded_scheduler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sptr_magic.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_decimator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_interpolator.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tpb_detail.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tpb_thread_body.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_timer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sys_paths.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_types.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_unittests.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_vmcircbuf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tags.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Install swig headers
########################################################################
if(ENABLE_PYTHON)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_basic_block.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_block_detail.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_hier_block2.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_buffer.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_dispatcher.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_error_handler.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_io_signature.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_message.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_handler.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_msg_queue.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_realtime.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_single_threaded_scheduler.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_block.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_decimator.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_sync_interpolator.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_tags.i
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_top_block.i
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "core_swig"
)
endif(ENABLE_PYTHON)
