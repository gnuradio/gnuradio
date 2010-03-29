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
#include "utils.h"

/***********************************************************************
 * Make UHD Source
 **********************************************************************/
boost::shared_ptr<uhd_simple_source> uhd_make_simple_source(
    const std::string &args,
    const std::string &type
){
    return boost::shared_ptr<uhd_simple_source>(
        new uhd_simple_source(args, type)
    );
}

/***********************************************************************
 * UHD Source
 **********************************************************************/
uhd_simple_source::uhd_simple_source(
    const std::string &args,
    const std::string &type
) : gr_sync_block(
    "uhd source",
    gr_make_io_signature(0, 0, 0),
    gr_make_io_signature(1, 1, get_size(type))
){
    _type = type;
    _dev = uhd::simple_device::make(args);
    _sizeof_samp = get_size(type);

    set_streaming(false);
}

uhd_simple_source::~uhd_simple_source(void){
    set_streaming(false);
}

void uhd_simple_source::set_streaming(bool enb){
    uhd::stream_cmd_t stream_cmd;
    stream_cmd.stream_now = true;
    stream_cmd.continuous = enb;
    _dev->issue_stream_cmd(stream_cmd);
    _is_streaming = enb;
}

void uhd_simple_source::set_samp_rate(double rate){
    return _dev->set_rx_rate(rate);
}

double uhd_simple_source::get_samp_rate(void){
    return _dev->get_rx_rate();
}

uhd::tune_result_t uhd_simple_source::set_center_freq(double freq){
    return _dev->set_rx_freq(freq);
}

/***********************************************************************
 * Work
 **********************************************************************/
int uhd_simple_source::work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    //conditionally start streaming in the work call
    //this prevents streaming before the runtime is ready
    if (not _is_streaming) set_streaming(true);

    size_t total_items_read = 0;
    uhd::rx_metadata_t metadata;

    //call until the output items are all filled
    //or an exit condition below is encountered
    while(total_items_read < size_t(noutput_items)){
        size_t items_read = _dev->get_device()->recv(
            boost::asio::buffer(
                (uint8_t *)output_items[0]+(total_items_read*_sizeof_samp),
                (noutput_items-total_items_read)*_sizeof_samp
            ), metadata, _type
        );
        total_items_read += items_read;

        //we timed out, get out of here
        if (items_read == 0) break;
    }

    return total_items_read;
}
