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

#include "amsg_source_impl.h"
#include "gr_uhd_common.h"
#include <boost/bind.hpp>

namespace gr {
  namespace uhd {

    amsg_source::sptr
    amsg_source::make(const ::uhd::device_addr_t &device_addr,
                      msg_queue::sptr msgq)
    {
      check_abi();
      return amsg_source::sptr
        (new amsg_source_impl(device_addr, msgq));
    }

    ::uhd::async_metadata_t
    amsg_source::msg_to_async_metadata_t(const message::sptr msg)
    {
      return *(::uhd::async_metadata_t *)msg->msg();
    }

    amsg_source_impl::amsg_source_impl(const ::uhd::device_addr_t &device_addr,
                                       msg_queue::sptr msgq)
      : _msgq(msgq), _running(true)
    {
      _dev = ::uhd::usrp::multi_usrp::make(device_addr);
      _amsg_thread =
        gr::thread::thread(boost::bind(&amsg_source_impl::recv_loop, this));
    }

    amsg_source_impl::~amsg_source_impl()
    {
      _running = false;
      _amsg_thread.join();
    }

    void
    amsg_source_impl::recv_loop()
    {
      message::sptr msg;
      ::uhd::async_metadata_t *md;

      while(_running) {
        msg = message::make(0, 0.0, 0.0, sizeof(::uhd::async_metadata_t));
        md = (::uhd::async_metadata_t*) msg->msg();

        while(!_dev->get_device()->recv_async_msg(*md, 0.1)) {
          if(!_running)
            return;
        }

        post(msg);
      }
    }

    void
    amsg_source_impl::post(message::sptr msg)
    {
      _msgq->insert_tail(msg);
    }

  } /* namespace uhd */
} /* namespace gr */
