/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2012-2013 Free Software Foundation, Inc.
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

#include "message_synch_impl.h"
#include <gnuradio/io_signature.h>
#include <cstdio>
#include <iostream>

namespace gr {
  namespace blocks {

    message_synch::sptr
    message_synch::make(int n_inputs)
    {
      return gnuradio::get_initial_sptr
        (new message_synch_impl(n_inputs));
    }

    message_synch_impl::message_synch_impl(int n_inputs)
      : block("message_synch",
                 io_signature::make(0, 0, 0),
                 io_signature::make(0, 0, 0))
    {
      for(int i=0; i<n_inputs; i++){
        d_queue.push_back( std::deque< pmt::pmt_t >() );
        std::string portname( (boost::format( "in%d" ) % i ).str() );
        std::cout << "register port: " << portname << "\n";
        message_port_register_in(pmt::mp(portname));
        set_msg_handler(pmt::mp(portname), boost::bind(&message_synch_impl::handler, this, i, _1));
        }
      message_port_register_out(pmt::mp("out"));
    }

    void message_synch_impl::handler(int n, pmt::pmt_t msg)
    {
        d_queue[n].push_back(msg);

        // return if we have any empty fifos
        for(size_t i=0; i<d_queue.size(); i++){
            if(d_queue[i].empty())
                return;
        }

        // push an N-tuple out off the front of each fifo
        pmt::pmt_t lst = pmt::list1(d_queue[0].front());
        d_queue[0].pop_front();
        for(size_t i=1; i<d_queue.size(); i++){
            lst = pmt::list_add(lst, d_queue[i].front());    
            d_queue[i].pop_front();
            }    
        message_port_pub(pmt::mp("out"), pmt::to_tuple(lst));
    }

    message_synch_impl::~message_synch_impl()
    {
    }

  } /* namespace blocks */
} /* namespace gr */

