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
#include <boost/thread.hpp>
#include <stdexcept>
#include "utils.h"

/***********************************************************************
 * Helper Functions
 **********************************************************************/
static size_t get_size(const std::string &type){
    if(type == "32fc"){
        return sizeof(std::complex<float>);
    }
    if(type == "16sc"){
        return sizeof(std::complex<short>);
    }
    throw std::runtime_error("unknown type");
}

/***********************************************************************
 * Make UHD Source
 **********************************************************************/
boost::shared_ptr<uhd_simple_source> uhd_make_simple_source(
    const std::string &args,
    const std::string &type
){
    return boost::shared_ptr<uhd_simple_source>(
        new uhd_simple_source(args_to_device_addr(args), type)
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
    gr_make_io_signature(1, 1, get_size(type))
){
    _type = type;
    _dev = uhd::device::make(addr);
    _sizeof_samp = get_size(type);

    set_streaming(true);
}

uhd_simple_source::~uhd_simple_source(void){
    set_streaming(false);
}

/***********************************************************************
 * DDC Control
 **********************************************************************/
void uhd_simple_source::set_streaming(bool enb){
    wax::obj ddc = (*_dev)
        [uhd::DEVICE_PROP_MBOARD]
        [uhd::named_prop_t(uhd::MBOARD_PROP_RX_DSP, "ddc0")];
    ddc[std::string("enabled")] = enb;
}

/***********************************************************************
 * Work
 **********************************************************************/
int uhd_simple_source::work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){

    const size_t max_samples = wax::cast<size_t>((*_dev)[uhd::DEVICE_PROP_MAX_RX_SAMPLES]);

    size_t total_items_read = 0;
    size_t count = 0;
    uhd::metadata_t metadata;
    while(total_items_read == 0 or total_items_read + max_samples < size_t(noutput_items)){
        size_t items_read = _dev->recv(
            boost::asio::buffer(
                (uint8_t *)output_items[0]+(total_items_read*_sizeof_samp),
                (noutput_items-total_items_read)*_sizeof_samp
            ), metadata, _type
        );

        //record items read and recv again
        if (items_read > 0){
            total_items_read += items_read;
            continue;
        }

        //if we have read at least once, but not this time, get out
        if (total_items_read > 0) break;

        //the timeout part
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        if (++count > 50) break;
    }
    
    return total_items_read;
}
