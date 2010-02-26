/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include <uhd_simple_source.h>
#include <gr_io_signature.h>
#include <stdexcept>

/***********************************************************************
 * Helper Functions
 **********************************************************************/
static gr_io_signature_sptr make_io_sig(const std::string &type){
    if(type == "32fc"){
        return gr_make_io_signature(1, 1, sizeof(std::complex<float>));
    }
    if(type == "16sc"){
        return gr_make_io_signature(1, 1, sizeof(std::complex<short>));
    }
    throw std::runtime_error("unknown type");
}

/***********************************************************************
 * Make UHD Source
 **********************************************************************/
boost::shared_ptr<uhd_simple_source> uhd_make_simple_source(
    const uhd::device_addr_t &addr,
    const std::string &type
){
    return boost::shared_ptr<uhd_simple_source>(
        new uhd_simple_source(addr, type)
    );
}

/***********************************************************************
 * UHD Source
 **********************************************************************/
uhd_simple_source::uhd_simple_source(
    const uhd::device_addr_t &addr,
    const std::string &type
) : gr_sync_block(
    "uhd source",
    gr_make_io_signature(0, 0, 0),
    make_io_sig(type)
){
    _type = type;
    _dev = uhd::device::make(addr);
}

uhd_simple_source::~uhd_simple_source(void){
    /* NOP */
}

/***********************************************************************
 * Work
 **********************************************************************/
int uhd_simple_source::work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    uhd::metadata_t metadata;
    return _dev->recv(boost::asio::buffer(output_items[0], noutput_items), metadata, _type);
}
