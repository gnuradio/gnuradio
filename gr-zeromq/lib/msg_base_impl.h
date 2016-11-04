/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GNURADIO_ZEROMQ_MSG_BASE_IMPL_H
#define INCLUDED_GNURADIO_ZEROMQ_MSG_BASE_IMPL_H

#include <zmq.hpp>
#include <gnuradio/zeromq/msg_base.h>

namespace gr {
  namespace zeromq {

    /*
     * The common base implementation
     */
    class msg_base_impl : virtual public msg_base
    {
    public:
      virtual ~msg_base_impl();
      std::string endpoint();
      bool d_zmq_finished; // public in old sub_msg_source -- should it be?

    protected:
      msg_base_impl(int type, char *address, int timeout=100);
      void handler(pmt::pmt_t msg);
      virtual bool start();
      virtual bool stop();
      virtual void readloop();
      int type;
      gr::thread::mutex d_mutex;
      boost::thread* d_thread;
      float d_timeout;
      zmq::context_t* d_context;
      zmq::socket_t* d_socket;
    };

    /*
     * The base sink implementation
     */
    class msg_base_sink_impl : public msg_base_impl
    {
    public:
      std::string endpoint() {return msg_base_impl::endpoint();};
      void set_endpoint(const char* address);

    protected:
      msg_base_sink_impl(int type, char *address, int timeout=100);
    };

    /*
     * The base source implementation
     */
    class msg_base_source_impl : public msg_base_impl
    {
    public:
      std::string endpoint() {return msg_base_impl::endpoint();};
      void set_endpoint(const char* address);

    protected:
      msg_base_source_impl(int type, char *address, int timeout=100);
    };

  }
}

#endif //INCLUDED_GNURADIO_MSG_BASE_IMPL_H