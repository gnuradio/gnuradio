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

#include <uhd_multi_usrp_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>

/***********************************************************************
 * UHD Multi USRP Sink
 **********************************************************************/
uhd_multi_usrp_sink::uhd_multi_usrp_sink(gr_io_signature_sptr sig)
:gr_sync_block("uhd multi usrp sink", sig, gr_make_io_signature(0, 0, 0)){
    /* NOP */
}

/***********************************************************************
 * UHD Multi USRP Sink Impl
 **********************************************************************/
class uhd_multi_usrp_sink_impl : public uhd_multi_usrp_sink{
public:
    uhd_multi_usrp_sink_impl(
        const std::string &device_addr,
        const uhd::io_type_t &io_type,
        size_t num_channels
    ):
        uhd_multi_usrp_sink(gr_make_io_signature(
            num_channels, num_channels, io_type.size
        )),
        _type(io_type),
        _nchan(num_channels)
    {
        _dev = uhd::usrp::multi_usrp::make(device_addr);
    }

    void set_subdev_spec(const std::string &spec, size_t mboard){
        return _dev->set_tx_subdev_spec(spec, mboard);
    }

    void set_samp_rate(double rate){
        _dev->set_tx_rate(rate);
    }

    double get_samp_rate(void){
        return _dev->get_tx_rate();
    }

    uhd::tune_result_t set_center_freq(double freq, size_t chan){
        uhd::tune_result_t tr = _dev->set_tx_freq(freq, chan);
        return tr;
    }

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_tx_freq_range(chan);
    }

    void set_gain(float gain, size_t chan){
        return _dev->set_tx_gain(gain, chan);
    }

    float get_gain(size_t chan){
        return _dev->get_tx_gain(chan);
    }

    uhd::gain_range_t get_gain_range(size_t chan){
        return _dev->get_tx_gain_range(chan);
    }

    void set_antenna(const std::string &ant, size_t chan){
        return _dev->set_tx_antenna(ant, chan);
    }

    std::string get_antenna(size_t chan){
        return _dev->get_tx_antenna(chan);
    }

    std::vector<std::string> get_antennas(size_t chan){
        return _dev->get_tx_antennas(chan);
    }

    void set_clock_config(const uhd::clock_config_t &clock_config, size_t mboard){
        return _dev->set_clock_config(clock_config, mboard);
    }

    uhd::time_spec_t get_time_now(void){
        return _dev->get_time_now();
    }

    void set_time_next_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_next_pps(time_spec);
    }

    void set_time_unknown_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_unknown_pps(time_spec);
    }

    uhd::usrp::multi_usrp::sptr get_device(void){
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

    //Send an empty start-of-burst packet to begin streaming.
    //Set at a time in the near future so data will be sync'd.
    bool start(void){
        uhd::tx_metadata_t metadata;
        metadata.start_of_burst = true;
        metadata.has_time_spec = true;
        metadata.time_spec = get_time_now() + uhd::time_spec_t(0.01); //10ms offset in future

        _dev->get_device()->send(
            gr_vector_const_void_star(_nchan), 0, metadata,
            _type, uhd::device::SEND_MODE_ONE_PACKET
        );
        return true;
    }

    //Send an empty end-of-burst packet to end streaming.
    //Ending the burst avoids an underflow error on stop.
    bool stop(void){
        uhd::tx_metadata_t metadata;
        metadata.end_of_burst = true;

        _dev->get_device()->send(
            gr_vector_const_void_star(_nchan), 0, metadata,
            _type, uhd::device::SEND_MODE_ONE_PACKET
        );
        return true;
    }

protected:
    uhd::usrp::multi_usrp::sptr _dev;
    const uhd::io_type_t _type;
    size_t _nchan;
};

/***********************************************************************
 * Make UHD Multi USRP Sink
 **********************************************************************/
boost::shared_ptr<uhd_multi_usrp_sink> uhd_make_multi_usrp_sink(
    const std::string &device_addr,
    const uhd::io_type_t::tid_t &io_type,
    size_t num_channels
){
    return boost::shared_ptr<uhd_multi_usrp_sink>(
        new uhd_multi_usrp_sink_impl(device_addr, io_type, num_channels)
    );
}
