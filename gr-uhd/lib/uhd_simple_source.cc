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
 * UHD Source
 **********************************************************************/
uhd_simple_source::uhd_simple_source(gr_io_signature_sptr sig)
:gr_sync_block("uhd source", gr_make_io_signature(0, 0, 0), sig){
    /* NOP */
}

/***********************************************************************
 * UHD Source Impl
 **********************************************************************/
class uhd_simple_source_impl : public uhd_simple_source{
public:
    uhd_simple_source_impl(
        const std::string &args,
        const uhd::io_type_t &type
    ) : uhd_simple_source(gr_make_io_signature(1, 1, type.size)), _type(type)
    {
        _dev = uhd::usrp::simple_usrp::make(args);
        set_streaming(false);
    }

    ~uhd_simple_source_impl(void){
        set_streaming(false);
    }

    void set_samp_rate(double rate){
        _dev->set_rx_rate(rate);
        do_samp_rate_error_message(rate, get_samp_rate());
    }

    double get_samp_rate(void){
        return _dev->get_rx_rate();
    }

    uhd::tune_result_t set_center_freq(double freq){
        return _dev->set_rx_freq(freq);
    }

    uhd::freq_range_t get_freq_range(void){
        return _dev->get_rx_freq_range();
    }

    void set_gain(float gain){
        return _dev->set_rx_gain(gain);
    }

    float get_gain(void){
        return _dev->get_rx_gain();
    }

    uhd::gain_range_t get_gain_range(void){
        return _dev->get_rx_gain_range();
    }

    void set_antenna(const std::string &ant){
        return _dev->set_rx_antenna(ant);
    }

    std::string get_antenna(void){
        return _dev->get_rx_antenna();
    }

    std::vector<std::string> get_antennas(void){
        return _dev->get_rx_antennas();
    }

    uhd::usrp::simple_usrp::sptr get_device(void){
        return _dev;
    }

/***********************************************************************
 * Work
 **********************************************************************/
    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    ){
        //conditionally start streaming in the work call
        //this prevents streaming before the runtime is ready
        if (not _is_streaming) set_streaming(true);

        uhd::rx_metadata_t metadata; //not passed out of this block

        return _dev->get_device()->recv(
            boost::asio::buffer(output_items[0], noutput_items*_type.size),
            metadata, _type, uhd::device::RECV_MODE_FULL_BUFF
        );
    }

private:
    uhd::usrp::simple_usrp::sptr _dev;
    const uhd::io_type_t _type;
    bool _is_streaming;

    void set_streaming(bool enb){
        if (enb)
            _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        else
            _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        _is_streaming = enb;
    }
};


/***********************************************************************
 * Make UHD Source
 **********************************************************************/
boost::shared_ptr<uhd_simple_source> uhd_make_simple_source(
    const std::string &args,
    const uhd::io_type_t::tid_t &type
){
    return boost::shared_ptr<uhd_simple_source>(
        new uhd_simple_source_impl(args, type)
    );
}
