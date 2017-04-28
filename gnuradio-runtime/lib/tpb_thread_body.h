/* -*- c++ -*- */
/*
 * Copyright 2008,2013 Free Software Foundation, Inc.
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

#include <gnuradio/api.h>
#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#include <gnuradio/thread/thread.h>
#include "block_executor.h"

namespace gr {

  /*!
   * \brief The body of each thread-per-block thread.
   *
   * One of these is instantiated in its own thread for each block.
   * The constructor turns into the main loop which returns when the
   * block is done or is interrupted.
   */
  class GR_RUNTIME_API tpb_thread_body
  {
    block_executor d_exec;

  public:
    tpb_thread_body(block_sptr block,  thread::barrier_sptr start_sync, int max_noutput_items=100000);
    ~tpb_thread_body();
  };

} /* namespace gr */

#endif /* INCLUDED_GR_TPB_THREAD_BODY_H */
