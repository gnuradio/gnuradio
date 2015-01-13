/* -*- c++ -*- */
/*
 * Copyright 2005,2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_MESSAGE_SYNCH_IMPL_H
#define INCLUDED_GR_MESSAGE_SYNCH_IMPL_H

#include <gnuradio/blocks/message_synch.h>
#include <gnuradio/block.h>
#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>

namespace gr {
  namespace blocks {

    class message_synch_impl : public message_synch
    {
    private:
      std::vector< std::deque< pmt::pmt_t > > d_queue;
      void handler(int n, pmt::pmt_t msg);

    public:
      message_synch_impl(int n_inputs);
      ~message_synch_impl();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_MESSAGE_SYNCH_IMPL_H */
