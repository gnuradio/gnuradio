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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_IO_H
#include <io.h>
#endif

#ifdef HAVE_WINDOWS_H
#include <winsock2.h>
#endif

#include <gnuradio/blocks/pdu.h>
#include <gnuradio/basic_block.h>
#include "stream_pdu_base.h"
#include <boost/format.hpp>

static const long timeout_us = 100*1000; //100ms

namespace gr {
  namespace blocks {

    stream_pdu_base::stream_pdu_base(int MTU)
      :	d_fd(-1),
	d_started(false),
	d_finished(false)
    {
      // reserve space for rx buffer
      d_rxbuf.resize(MTU,0);
    }

    stream_pdu_base::~stream_pdu_base()
    {
      stop_rxthread();
    }

    void
    stream_pdu_base::start_rxthread(basic_block *blk, pmt::pmt_t port)
    {
      d_blk = blk;
      d_port = port;
      d_thread = gr::thread::thread(boost::bind(&stream_pdu_base::run, this));
      d_started = true;
    }

    void
    stream_pdu_base::stop_rxthread()
    {
      d_finished = true;

      if (d_started) {
        d_thread.interrupt();
        d_thread.join();
      }
    }

    void
    stream_pdu_base::run()
    {
      while(!d_finished) {
        if (!wait_ready())
	  continue;

        const int result = read(d_fd, &d_rxbuf[0], d_rxbuf.size());
        if (result <= 0)
	  throw std::runtime_error("stream_pdu_base, bad socket read!");

        pmt::pmt_t vector = pmt::init_u8vector(result, &d_rxbuf[0]);
        pmt::pmt_t pdu = pmt::cons(pmt::PMT_NIL, vector);

        d_blk->message_port_pub(d_port, pdu);
      }
    }

    bool
    stream_pdu_base::wait_ready()
    {
      //setup timeval for timeout
      timeval tv;
      tv.tv_sec = 0;
      tv.tv_usec = timeout_us;

      //setup rset for timeout
      fd_set rset;
      FD_ZERO(&rset);
      FD_SET(d_fd, &rset);

      //call select with timeout on receive socket
      return ::select(d_fd+1, &rset, NULL, NULL, &tv) > 0;
    }

    void
    stream_pdu_base::send(pmt::pmt_t msg)
    {
      pmt::pmt_t vector = pmt::cdr(msg);
      size_t offset(0);
      size_t itemsize(pdu::itemsize(pdu::type_from_pmt(vector)));
      int len(pmt::length(vector)*itemsize);

      const int rv = write(d_fd, pmt::uniform_vector_elements(vector, offset), len);
      if (rv != len) {
        std::cerr << boost::format("WARNING: stream_pdu_base::send(pdu) write failed! (d_fd=%d, len=%d, rv=%d)")
	  % d_fd % len % rv << std::endl;
      }
    }

  } /* namespace blocks */
} /* namespace gr */
