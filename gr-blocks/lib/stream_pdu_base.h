/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_STREAM_PDU_BASE_H
#define INCLUDED_STREAM_PDU_BASE_H

#include <gnuradio/thread/thread.h>
#include <pmt/pmt.h>

class basic_block;

namespace gr {
  namespace blocks {

    class stream_pdu_base
    {
    public:
      stream_pdu_base(int MTU=10000);
      ~stream_pdu_base();

    protected:
      int d_fd;
      bool d_started;
      bool d_finished;
      std::vector<uint8_t> d_rxbuf;
      gr::thread::thread d_thread;

      pmt::pmt_t d_port;
      basic_block *d_blk;

      void run();
      void send(pmt::pmt_t msg);
      bool wait_ready();
      void start_rxthread(basic_block *blk, pmt::pmt_t rxport);
      void stop_rxthread();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_STREAM_PDU_BASE_H */
