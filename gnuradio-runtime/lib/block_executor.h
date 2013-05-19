/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_RUNTIME_BLOCK_EXECUTOR_H
#define INCLUDED_GR_RUNTIME_BLOCK_EXECUTOR_H

#include <gnuradio/api.h>
#include <gnuradio/runtime_types.h>
#include <gnuradio/tags.h>
#include <fstream>

namespace gr {

  /*!
   * \brief Manage the execution of a single block.
   * \ingroup internal
   */
  class GR_RUNTIME_API block_executor
  {
  protected:
    block_sptr     d_block;    // The block we're trying to run
    std::ofstream *d_log;

    // These are allocated here so we don't have to on each iteration

    gr_vector_int		d_ninput_items_required;
    gr_vector_int		d_ninput_items;
    gr_vector_const_void_star	d_input_items;
    std::vector<bool>		d_input_done;
    gr_vector_void_star		d_output_items;
    std::vector<uint64_t>       d_start_nitems_read; //stores where tag counts are before work
    std::vector<tag_t>          d_returned_tags;
    int                         d_max_noutput_items;

#ifdef GR_PERFORMANCE_COUNTERS
    bool d_use_pc;
#endif /* GR_PERFORMANCE_COUNTERS */

  public:
    block_executor(block_sptr block, int max_noutput_items=100000);
    ~block_executor();

    enum state {
      READY,	        // We made progress; everything's cool.
      READY_NO_OUTPUT,  // We consumed some input, but produced no output.
      BLKD_IN,	        // no progress; we're blocked waiting for input data.
      BLKD_OUT,	        // no progress; we're blocked waiting for output buffer space.
      DONE,	        // we're done; don't call me again.
    };

    /*
     * \brief Run one iteration.
     */
    state run_one_iteration();
  };

} /* namespace gr */

#endif /* INCLUDED_GR_RUNTIME_BLOCK_EXECUTOR_H */
