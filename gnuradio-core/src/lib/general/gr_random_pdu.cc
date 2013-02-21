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

#include <gr_random_pdu.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>
#include <iostream>
#include <vector>

// public constructor that returns a shared_ptr

gr_random_pdu_sptr
gr_make_random_pdu (int items_min, int items_max)
{
  return gnuradio::get_initial_sptr(new gr_random_pdu(items_min, items_max));
}

gr_random_pdu::gr_random_pdu (int items_min, int items_max)
  : gr_block("random_pdu",
	     gr_make_io_signature(0, 0, 0),
	     gr_make_io_signature(0, 0, 0)),
    urange(items_min, items_max),
    brange(0, 255),
    rvar(rng, urange),
    bvar(rng, brange)
{
    message_port_register_out(pmt::mp("pdus"));
    message_port_register_in(pmt::mp("generate"));
    set_msg_handler(pmt::mp("generate"), boost::bind(&gr_random_pdu::generate_pdu, this, _1));
}

bool gr_random_pdu::start(){
    output_random();
    return true;
}

void gr_random_pdu::output_random(){

    // pick a random vector length
    int len = rvar();

    // fill it with random bytes
    std::vector<unsigned char> vec;
    for(int i=0; i<len; i++){
        vec.push_back((unsigned char) bvar());
        }

    // send the vector
    pmt::pmt_t vecpmt( pmt::pmt_make_blob( &vec[0], len ) );
    pmt::pmt_t pdu( pmt::pmt_cons( pmt::PMT_NIL, vecpmt ) );
    message_port_pub( pmt::mp("pdus"), pdu );
    
    std::cout << "sending new random vector of length " << len << "\n";
}

