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

#include <uhd_mimo_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include "utils.h"

/***********************************************************************
 * UHD Sink
 **********************************************************************/
uhd_mimo_sink::uhd_mimo_sink(gr_io_signature_sptr sig)
:gr_sync_block("uhd mimo sink", sig, gr_make_io_signature(0, 0, 0)){
    /* NOP */
}

/***********************************************************************
 * UHD Sink Impl
 **********************************************************************/
class uhd_mimo_sink_impl : public uhd_mimo_sink{
public:
    uhd_mimo_sink_impl(
        size_t num_channels,
        const std::string &args,
        const uhd::io_type_t &type
    ) : uhd_mimo_sink(gr_make_io_signature(num_channels, num_channels, type.size)), _type(type)
    {
        _first_run = false;
        _dev = uhd::usrp::mimo_usrp::make(args);
        _dev->set_time_unknown_pps(uhd::time_spec_t()); //TODO may want option to disable this
    }

    ~uhd_mimo_sink_impl(void){
        //NOP
    }

    void set_samp_rate_all(double rate){
        _dev->set_tx_rate_all(rate);
        do_samp_rate_error_message(rate, get_samp_rate_all(), "TX");
    }

    double get_samp_rate_all(void){
        return _dev->get_tx_rate_all();
    }

    uhd::tune_result_t set_center_freq(size_t chan, double freq){
        uhd::tune_result_t tr = _dev->set_tx_freq(chan, freq);
        do_tune_freq_error_message(freq, _dev->get_tx_freq(chan), "TX");
        return tr;
    }

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_tx_freq_range(chan);
    }

    void set_gain(size_t chan, float gain){
        return _dev->set_tx_gain(chan, gain);
    }

    float get_gain(size_t chan){
        return _dev->get_tx_gain(chan);
    }

    uhd::gain_range_t get_gain_range(size_t chan){
        return _dev->get_tx_gain_range(chan);
    }

    void set_antenna(size_t chan, const std::string &ant){
        return _dev->set_tx_antenna(chan, ant);
    }

    std::string get_antenna(size_t chan){
        return _dev->get_tx_antenna(chan);
    }

    std::vector<std::string> get_antennas(size_t chan){
        return _dev->get_tx_antennas(chan);
    }

    uhd::time_spec_t get_time_now(void){
        return _dev->get_time_now();
    }

    void set_time_next_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_next_pps(time_spec);
    }

    uhd::usrp::mimo_usrp::sptr get_device(void){
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
        //init the metadata on the first call to work
        //and set the timespec with the current time + some offset
        if (not _first_run){
            _first_run = true;
            _metadata.start_of_burst = true;
            _metadata.has_time_spec = true;
            _metadata.time_spec = get_time_now() + uhd::time_spec_t(0, 0.01); //10ms offset in future
        }

        //call to send with metadata slightly in the future (send-at)
        size_t num_sent = _dev->get_device()->send(
            input_items, noutput_items, _metadata,
            _type, uhd::device::SEND_MODE_FULL_BUFF
        );

        //increment the send-at time by the number of samples sent
        _metadata.time_spec += uhd::time_spec_t(0, num_sent, get_samp_rate_all());
        return num_sent;
    }

protected:
    uhd::usrp::mimo_usrp::sptr _dev;
    const uhd::io_type_t _type;
    uhd::tx_metadata_t _metadata;
    bool _first_run;
};

/***********************************************************************
 * Make UHD Sink
 **********************************************************************/
boost::shared_ptr<uhd_mimo_sink> uhd_make_mimo_sink(
    size_t num_channels,
    const std::string &args,
    const uhd::io_type_t::tid_t &type
){
    return boost::shared_ptr<uhd_mimo_sink>(
        new uhd_mimo_sink_impl(num_channels, args, type)
    );
}
