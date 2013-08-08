/* -*- c++ -*- */
/*
 * Copyright 2011-2013 Free Software Foundation, Inc.
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

#include <gnuradio/uhd/amsg_source.h>
#include <gnuradio/thread/thread.h>

namespace gr {
  namespace uhd {

    class amsg_source_impl : public amsg_source
    {
    public:
      amsg_source_impl(const ::uhd::device_addr_t &device_addr,
                       msg_queue::sptr msgq);
      ~amsg_source_impl();

      void recv_loop();
      void post(message::sptr msg);

    protected:
      ::uhd::usrp::multi_usrp::sptr _dev;
      gr::thread::thread _amsg_thread;
      msg_queue::sptr _msgq;
      bool _running;
    };

  } /* namespace uhd */
} /* namespace gr */

