/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

%{
#include <gr_runtime_types.h>
#include <gr_io_signature.h>
#include <gr_buffer.h>
#include <gr_block.h>
#include <gr_block_detail.h>
#include <gr_hier_block2.h>
#include <gr_single_threaded_scheduler.h>
#include <gr_message.h>
#include <gr_msg_handler.h>
#include <gr_msg_queue.h>
#include <gr_dispatcher.h>
#include <gr_error_handler.h>
#include <gr_realtime.h>
#include <gr_sync_block.h>
#include <gr_sync_decimator.h>
#include <gr_sync_interpolator.h>
#include <gr_top_block.h>
%}

%include <gr_io_signature.i>
%include <gr_buffer.i>
%include <gr_basic_block.i>
%include <gr_block.i>
%include <gr_block_detail.i>
%include <gr_hier_block2.i>
%include <gr_swig_block_magic.i>
%include <gr_single_threaded_scheduler.i>
%include <gr_message.i>
%include <gr_msg_handler.i>
%include <gr_msg_queue.i>
%include <gr_dispatcher.i>
%include <gr_error_handler.i>
%include <gr_realtime.i>
%include <gr_sync_block.i>
%include <gr_sync_decimator.i>
%include <gr_sync_interpolator.i>
%include <gr_top_block.i>
