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

#include <uhd_multi_usrp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>

/***********************************************************************
 * UHD Multi USRP Source
 **********************************************************************/
uhd_multi_usrp_source::uhd_multi_usrp_source(gr_io_signature_sptr sig)
:gr_sync_block("uhd multi_usrp source", gr_make_io_signature(0, 0, 0), sig){
    /* NOP */
}

/***********************************************************************
 * UHD Multi USRP Source Impl
 **********************************************************************/
class uhd_multi_usrp_source_impl : public uhd_multi_usrp_source{
public:
    uhd_multi_usrp_source_impl(
        const std::string &args,
        const uhd::io_type_t &type,
        size_t num_channels
    ) : uhd_multi_usrp_source(gr_make_io_signature(num_channels, num_channels, type.size)), _type(type)
    {
        _dev = uhd::usrp::multi_usrp::make(args);
    }

    void set_subdev_spec(const std::string &spec, size_t mboard){
        return _dev->set_rx_subdev_spec(spec, mboard);
    }

    void set_samp_rate(double rate){
        _dev->set_rx_rate(rate);
    }

    double get_samp_rate(void){
        return _dev->get_rx_rate();
    }

    uhd::tune_result_t set_center_freq(double freq, size_t chan){
        uhd::tune_result_t tr = _dev->set_rx_freq(freq, chan);
        return tr;
    }

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_rx_freq_range(chan);
    }

    void set_gain(float gain, size_t chan){
        return _dev->set_rx_gain(gain, chan);
    }

    float get_gain(size_t chan){
        return _dev->get_rx_gain(chan);
    }

    uhd::gain_range_t get_gain_range(size_t chan){
        return _dev->get_rx_gain_range(chan);
    }

    void set_antenna(const std::string &ant, size_t chan){
        return _dev->set_rx_antenna(ant, chan);
    }

    std::string get_antenna(size_t chan){
        return _dev->get_rx_antenna(chan);
    }

    std::vector<std::string> get_antennas(size_t chan){
        return _dev->get_rx_antennas(chan);
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
        uhd::rx_metadata_t metadata; //not passed out of this block

        size_t num_samps = _dev->get_device()->recv(
            output_items, noutput_items, metadata,
            _type, uhd::device::RECV_MODE_FULL_BUFF
        );

        switch(metadata.error_code){
        case uhd::rx_metadata_t::ERROR_CODE_NONE:
            return num_samps;

        case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
            //ignore overflows and try work again
            return work(noutput_items, input_items, output_items);

        default:
            std::cout << boost::format(
                "UHD source block got error code 0x%x"
            ) % metadata.error_code << std::endl;
            return num_samps;
        }
    }

    bool start(void){
        //setup a stream command that starts streaming slightly in the future
        static const double reasonable_delay = 0.01; //10 ms (order of magnitude >> RTT)
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        stream_cmd.stream_now = false;
        stream_cmd.time_spec = get_time_now() + uhd::time_spec_t(_dev->get_num_mboards() * reasonable_delay);
        _dev->issue_stream_cmd(stream_cmd);
        return true;
    }

    bool stop(void){
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        return true;
    }

private:
    uhd::usrp::multi_usrp::sptr _dev;
    const uhd::io_type_t _type;
};


/***********************************************************************
 * Make UHD Multi USRP Source
 **********************************************************************/
boost::shared_ptr<uhd_multi_usrp_source> uhd_make_multi_usrp_source(
    const std::string &args,
    const uhd::io_type_t::tid_t &type,
    size_t num_channels
){
    return boost::shared_ptr<uhd_multi_usrp_source>(
        new uhd_multi_usrp_source_impl(args, type, num_channels)
    );
}
