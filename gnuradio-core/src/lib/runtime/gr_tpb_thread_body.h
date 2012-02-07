/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_GR_TPB_THREAD_BODY_H
#define INCLUDED_GR_TPB_THREAD_BODY_H

#include <gr_core_api.h>
#include <gr_block_executor.h>
#include <gr_block.h>
#include <gr_block_detail.h>

/*!
 * \brief The body of each thread-per-block thread.
 *
 * One of these is instantiated in its own thread for each block.  The
 * constructor turns into the main loop which returns when the block is
 * done or is interrupted.
 */

class GR_CORE_API gr_tpb_thread_body {
  gr_block_executor	d_exec;

public:
  gr_tpb_thread_body(gr_block_sptr block, int max_noutput_items=100000);
  ~gr_tpb_thread_body();
};


#endif /* INCLUDED_GR_TPB_THREAD_BODY_H */
