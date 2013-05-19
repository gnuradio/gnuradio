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

#ifndef INCLUDED_GR_SINGLE_THREADED_SCHEDULER_H
#define INCLUDED_GR_SINGLE_THREADED_SCHEDULER_H

#include <gnuradio/api.h>
#include <gnuradio/runtime_types.h>
#include <fstream>

namespace gr {

  class single_threaded_scheduler;
  typedef boost::shared_ptr<single_threaded_scheduler> single_threaded_scheduler_sptr;

  /*!
   * \brief Simple scheduler for stream computations.
   * \ingroup internal
   */
  class GR_RUNTIME_API single_threaded_scheduler
  {
  public:
    ~single_threaded_scheduler();

    void run();
    void stop();

  private:
    const std::vector<block_sptr> d_blocks;
    volatile bool d_enabled;
    std::ofstream *d_log;

    single_threaded_scheduler(const std::vector<block_sptr> &blocks);

    void main_loop();

    friend GR_RUNTIME_API single_threaded_scheduler_sptr
      make_single_threaded_scheduler(const std::vector<block_sptr> &blocks);
  };

  GR_RUNTIME_API single_threaded_scheduler_sptr
  make_single_threaded_scheduler(const std::vector<block_sptr> &blocks);

} /* namespace gr */

#endif /* INCLUDED_GR_SINGLE_THREADED_SCHEDULER_H */
