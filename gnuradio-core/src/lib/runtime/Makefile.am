#
# Copyright 2003,2004 Free Software Foundation, Inc.
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
# 

include $(top_srcdir)/Makefile.common

INCLUDES = $(STD_DEFINES_AND_INCLUDES) $(CPPUNIT_INCLUDES)

noinst_LTLIBRARIES = libruntime.la libruntime-qa.la

libruntime_la_LIBADD =		\
	$(SHM_OPEN_LIBS)


libruntime_la_SOURCES = 			\
	gr_basic_block.cc			\
	gr_flowgraph.cc				\
	gr_flat_flowgraph.cc			\
	gr_block.cc				\
	gr_block_detail.cc			\
	gr_hier_block2.cc			\
	gr_hier_block2_detail.cc		\
	gr_buffer.cc				\
	gr_dispatcher.cc			\
	gr_error_handler.cc			\
	gr_io_signature.cc			\
	gr_local_sighandler.cc			\
	gr_message.cc				\
	gr_msg_handler.cc			\
	gr_msg_queue.cc				\
	gr_pagesize.cc				\
	gr_preferences.cc			\
	gr_realtime.cc				\
	gr_runtime.cc				\
	gr_runtime_impl.cc			\
	gr_single_threaded_scheduler.cc		\
	gr_sync_block.cc			\
	gr_sync_decimator.cc			\
	gr_sync_interpolator.cc			\
	gr_tmp_path.cc				\
	gr_vmcircbuf.cc				\
	gr_vmcircbuf_mmap_shm_open.cc		\
	gr_vmcircbuf_mmap_tmpfile.cc		\
	gr_vmcircbuf_createfilemapping.cc	\
	gr_vmcircbuf_sysv_shm.cc		\
	gr_select_handler.cc			

libruntime_qa_la_SOURCES = 			\
	qa_gr_block.cc				\
	qa_gr_hier_block2.cc			\
	qa_gr_buffer.cc				\
	qa_gr_flowgraph.cc			\
	qa_gr_io_signature.cc			\
	qa_gr_vmcircbuf.cc			\
	qa_runtime.cc				

grinclude_HEADERS = 				\
	gr_basic_block.h			\
	gr_flowgraph.h				\
	gr_flat_flowgraph.h			\
	gr_block.h				\
	gr_block_detail.h			\
	gr_hier_block2.h			\
	gr_hier_block2_detail.h			\
	gr_buffer.h				\
	gr_complex.h				\
	gr_dispatcher.h				\
	gr_error_handler.h			\
	gr_io_signature.h			\
	gr_local_sighandler.h			\
	gr_message.h				\
	gr_msg_handler.h			\
	gr_msg_queue.h				\
	gr_pagesize.h				\
	gr_preferences.h			\
	gr_realtime.h				\
	gr_runtime.h				\
	gr_runtime_impl.h			\
	gr_runtime_types.h			\
	gr_select_handler.h			\
	gr_single_threaded_scheduler.h		\
	gr_sync_block.h				\
	gr_sync_decimator.h			\
	gr_sync_interpolator.h			\
	gr_timer.h				\
	gr_tmp_path.h				\
	gr_types.h				\
	gr_vmcircbuf.h				

noinst_HEADERS = 				\
	gr_vmcircbuf_mmap_shm_open.h		\
	gr_vmcircbuf_mmap_tmpfile.h		\
	gr_vmcircbuf_sysv_shm.h			\
	gr_vmcircbuf_createfilemapping.h	\
	qa_gr_block.h				\
	qa_gr_flowgraph.h			\
	qa_gr_hier_block2.h			\
	qa_gr_buffer.h				\
	qa_gr_io_signature.h			\
	qa_gr_vmcircbuf.h			\
	qa_runtime.h				

swiginclude_HEADERS =			\
	gr_basic_block.i		\
	gr_block.i			\
	gr_block_detail.i		\
	gr_hier_block2.i		\
	gr_buffer.i			\
	gr_dispatcher.i			\
	gr_error_handler.i		\
	gr_io_signature.i		\
	gr_message.i			\
	gr_msg_handler.i		\
	gr_msg_queue.i			\
	gr_realtime.i			\
	gr_runtime.i			\
	gr_single_threaded_scheduler.i	\
	gr_sync_block.i			\
	gr_sync_decimator.i		\
	gr_sync_interpolator.i		\
	gr_swig_block_magic.i		\
	runtime.i

MOSTLYCLEANFILES = *~ *.loT
