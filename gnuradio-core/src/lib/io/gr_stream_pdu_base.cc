/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <ciso646>
#include <gr_stream_pdu_base.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <gr_pdu.h>
#include <boost/asio.hpp>
#include <boost/format.hpp>

#ifdef HAVE_IO_H
#include <io.h>
#endif

static const long timeout_us = 100*1000; //100ms

gr_stream_pdu_base::gr_stream_pdu_base (int MTU)
  : gr_sync_block("stream_pdu_base",
		  gr_make_io_signature(0, 0, 0),
		  gr_make_io_signature(0, 0, 0)),
    d_finished(false), d_started(false), d_fd(-1)
{
    // reserve space for rx buffer 
    d_rxbuf.resize(MTU,0);
}

gr_stream_pdu_base::~gr_stream_pdu_base()
{
    stop_rxthread();
}

void gr_stream_pdu_base::stop_rxthread(){
    d_finished = true;
    if(d_started){
        d_thread->interrupt();
        d_thread->join();
        }
    }

void gr_stream_pdu_base::start_rxthread(pmt::pmt_t _rxport){
    rxport = _rxport;
    d_thread = boost::shared_ptr<boost::thread>(new boost::thread(boost::bind(&gr_stream_pdu_base::run, this)));
    d_started = true;
    }

void gr_stream_pdu_base::run(){
    while(!d_finished) {
        if(not wait_ready()){ continue; }   
        const int result = read( d_fd, &d_rxbuf[0], d_rxbuf.size() );
        if(result <= 0){ throw std::runtime_error("gr_stream_pdu_base, bad socket read!"); }
        pmt::pmt_t vector = pmt::pmt_init_u8vector(result, &d_rxbuf[0]);       
        pmt::pmt_t pdu = pmt::pmt_cons( pmt::PMT_NIL, vector);
        message_port_pub(rxport, pdu);
    } 
}

void gr_stream_pdu_base::send(pmt::pmt_t msg){
    pmt::pmt_t vector = pmt::pmt_cdr(msg);
    size_t offset(0);
    size_t itemsize(gr_pdu_itemsize(type_from_pmt(vector)));
    int len( pmt::pmt_length(vector)*itemsize );
    
    const int rv = write(d_fd, pmt::pmt_uniform_vector_elements(vector, offset), len);
    if(rv != len){
        std::cerr << boost::format("WARNING: gr_stream_pdu_base::send(pdu) write failed! (d_fd=%d, len=%d, rv=%d)")
                            % d_fd % len % rv << std::endl;
        }
}

int
gr_stream_pdu_base::work(int noutput_items,
		      gr_vector_const_void_star &input_items,
		      gr_vector_void_star &output_items)
{
  throw std::runtime_error("should not be called.\n");
  return 0; 
}

bool gr_stream_pdu_base::wait_ready(){
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
