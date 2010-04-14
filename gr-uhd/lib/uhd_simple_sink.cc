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

#include <uhd_simple_sink.h>
#include <gr_io_signature.h>
#include <boost/thread.hpp>
#include <stdexcept>
#include "utils.h"

/***********************************************************************
 * Make UHD Sink
 **********************************************************************/
boost::shared_ptr<uhd_simple_sink> uhd_make_simple_sink(
    const std::string &args,
    const uhd::io_type_t::tid_t &type
){
    return boost::shared_ptr<uhd_simple_sink>(
        new uhd_simple_sink(args, type)
    );
}

/***********************************************************************
 * UHD Sink
 **********************************************************************/
uhd_simple_sink::uhd_simple_sink(
    const std::string &args,
    const uhd::io_type_t &type
) : gr_sync_block(
    "uhd sink",
    gr_make_io_signature(1, 1, type.size),
    gr_make_io_signature(0, 0, 0)
), _type(type){
    _dev = uhd::usrp::simple_usrp::make(args);
}

uhd_simple_sink::~uhd_simple_sink(void){
    //NOP
}

void uhd_simple_sink::set_samp_rate(double rate){
    _dev->set_tx_rate(rate);
    do_samp_rate_error_message(rate, get_samp_rate());
}

double uhd_simple_sink::get_samp_rate(void){
    return _dev->get_tx_rate();
}

uhd::tune_result_t uhd_simple_sink::set_center_freq(double freq){
    return _dev->set_tx_freq(freq);
}

uhd::freq_range_t uhd_simple_sink::get_freq_range(void){
    return _dev->get_tx_freq_range();
}

void uhd_simple_sink::set_gain(float gain){
    return _dev->set_tx_gain(gain);
}

float uhd_simple_sink::get_gain(void){
    return _dev->get_tx_gain();
}

uhd::gain_range_t uhd_simple_sink::get_gain_range(void){
    return _dev->get_tx_gain_range();
}

void uhd_simple_sink::set_antenna(const std::string &ant){
    return _dev->set_tx_antenna(ant);
}

std::string uhd_simple_sink::get_antenna(void){
    return _dev->get_tx_antenna();
}

std::vector<std::string> uhd_simple_sink::get_antennas(void){
    return _dev->get_tx_antennas();
}

/***********************************************************************
 * Work
 **********************************************************************/
int uhd_simple_sink::work(
    int noutput_items,
    gr_vector_const_void_star &input_items,
    gr_vector_void_star &output_items
){
    size_t total_items_sent = 0;
    uhd::tx_metadata_t metadata;
    metadata.start_of_burst = true;

    //call until the input items are all sent
    while(total_items_sent < size_t(noutput_items)){
        size_t items_sent = _dev->get_device()->send(
            boost::asio::buffer(
                (uint8_t *)input_items[0]+(total_items_sent*_type.size),
                (noutput_items-total_items_sent)*_type.size
            ), metadata, _type
        );
        total_items_sent += items_sent;
    }

    return noutput_items;
}
