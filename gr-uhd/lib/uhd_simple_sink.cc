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
#include <stdexcept>
#include "utils.h"

/***********************************************************************
 * UHD Sink
 **********************************************************************/
uhd_simple_sink::uhd_simple_sink(gr_io_signature_sptr sig)
:gr_sync_block("uhd simple sink", sig, gr_make_io_signature(0, 0, 0)){
    /* NOP */
}

/***********************************************************************
 * UHD Sink Impl
 **********************************************************************/
class uhd_simple_sink_impl : public uhd_simple_sink{
public:
    uhd_simple_sink_impl(
        const std::string &args,
        const uhd::io_type_t &type
    ) : uhd_simple_sink(gr_make_io_signature(1, 1, type.size)), _type(type)
    {
        _dev = uhd::usrp::simple_usrp::make(args);
    }

    ~uhd_simple_sink_impl(void){
        //NOP
    }

    void set_samp_rate(double rate){
        _dev->set_tx_rate(rate);
        do_samp_rate_error_message(rate, get_samp_rate(), "TX");
    }

    double get_samp_rate(void){
        return _dev->get_tx_rate();
    }

    uhd::tune_result_t set_center_freq(double freq){
        uhd::tune_result_t tr = _dev->set_tx_freq(freq);
        do_tune_freq_error_message(freq, _dev->get_tx_freq(), "TX");
        return tr;
    }

    uhd::freq_range_t get_freq_range(void){
        return _dev->get_tx_freq_range();
    }

    void set_gain(float gain){
        return _dev->set_tx_gain(gain);
    }

    float get_gain(void){
        return _dev->get_tx_gain();
    }

    uhd::gain_range_t get_gain_range(void){
        return _dev->get_tx_gain_range();
    }

    void set_antenna(const std::string &ant){
        return _dev->set_tx_antenna(ant);
    }

    std::string get_antenna(void){
        return _dev->get_tx_antenna();
    }

    std::vector<std::string> get_antennas(void){
        return _dev->get_tx_antennas();
    }

    void set_clock_config(const uhd::clock_config_t &clock_config){
        return _dev->set_clock_config(clock_config);
    }

    uhd::time_spec_t get_time_now(void){
        return _dev->get_time_now();
    }

    void set_time_now(const uhd::time_spec_t &time_spec){
        return _dev->set_time_now(time_spec);
    }

    void set_time_next_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_next_pps(time_spec);
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
        uhd::tx_metadata_t metadata;
        metadata.start_of_burst = true;

        return _dev->get_device()->send(
            input_items, noutput_items, metadata,
            _type, uhd::device::SEND_MODE_FULL_BUFF
        );
    }

protected:
    uhd::usrp::simple_usrp::sptr _dev;
    const uhd::io_type_t _type;
};

/***********************************************************************
 * Make UHD Sink
 **********************************************************************/
boost::shared_ptr<uhd_simple_sink> uhd_make_simple_sink(
    const std::string &args,
    const uhd::io_type_t::tid_t &type
){
    return boost::shared_ptr<uhd_simple_sink>(
        new uhd_simple_sink_impl(args, type)
    );
}
