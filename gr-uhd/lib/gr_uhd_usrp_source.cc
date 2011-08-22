/*
 * Copyright 2010-2011 Free Software Foundation, Inc.
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

#include <gr_uhd_usrp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>

/***********************************************************************
 * UHD Multi USRP Source Impl
 **********************************************************************/
class uhd_usrp_source_impl : public uhd_usrp_source{
public:
    uhd_usrp_source_impl(
        const uhd::device_addr_t &device_addr,
        const uhd::io_type_t &io_type,
        size_t num_channels
    ):
        gr_sync_block(
            "gr uhd usrp source",
            gr_make_io_signature(0, 0, 0),
            gr_make_io_signature(num_channels, num_channels, io_type.size)
        ),
        _type(io_type),
        _nchan(num_channels),
        _stream_now(_nchan == 1),
    {
        _dev = uhd::usrp::multi_usrp::make(device_addr);
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

    uhd::tune_result_t set_center_freq(
        const uhd::tune_request_t tune_request, size_t chan
    ){
        return _dev->set_rx_freq(tune_request, chan);
    }

    double get_center_freq(size_t chan){
        return _dev->get_rx_freq(chan);
    }

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_rx_freq_range(chan);
    }

    void set_gain(double gain, size_t chan){
        return _dev->set_rx_gain(gain, chan);
    }

    void set_gain(double gain, const std::string &name, size_t chan){
        return _dev->set_rx_gain(gain, name, chan);
    }

    double get_gain(size_t chan){
        return _dev->get_rx_gain(chan);
    }

    double get_gain(const std::string &name, size_t chan){
        return _dev->get_rx_gain(name, chan);
    }

    std::vector<std::string> get_gain_names(size_t chan){
        return _dev->get_rx_gain_names(chan);
    }

    uhd::gain_range_t get_gain_range(size_t chan){
        return _dev->get_rx_gain_range(chan);
    }

    uhd::gain_range_t get_gain_range(const std::string &name, size_t chan){
        return _dev->get_rx_gain_range(name, chan);
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

    void set_bandwidth(double bandwidth, size_t chan){
        return _dev->set_rx_bandwidth(bandwidth, chan);
    }

    uhd::sensor_value_t get_dboard_sensor(const std::string &name, size_t chan){
        return _dev->get_rx_sensor(name, chan);
    }

    std::vector<std::string> get_dboard_sensor_names(size_t chan){
        return _dev->get_rx_sensor_names(chan);
    }

    uhd::sensor_value_t get_mboard_sensor(const std::string &name, size_t mboard){
        return _dev->get_mboard_sensor(name, mboard);
    }

    std::vector<std::string> get_mboard_sensor_names(size_t mboard){
        return _dev->get_mboard_sensor_names(mboard);
    }

    void set_clock_config(const uhd::clock_config_t &clock_config, size_t mboard){
        return _dev->set_clock_config(clock_config, mboard);
    }

    double get_clock_rate(size_t mboard){
        return _dev->get_master_clock_rate(mboard);
    }

    void set_clock_rate(double rate, size_t mboard){
        return _dev->set_master_clock_rate(rate, mboard);
    }

    uhd::time_spec_t get_time_now(size_t mboard = 0){
        return _dev->get_time_now(mboard);
    }

    uhd::time_spec_t get_time_last_pps(size_t mboard){
        return _dev->get_time_last_pps(mboard);
    }

    void set_time_now(const uhd::time_spec_t &time_spec, size_t mboard){
        return _dev->set_time_now(time_spec, mboard);
    }

    void set_time_next_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_next_pps(time_spec);
    }

    void set_time_unknown_pps(const uhd::time_spec_t &time_spec){
        return _dev->set_time_unknown_pps(time_spec);
    }

    uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan){
        return _dev->get_rx_dboard_iface(chan);
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
        //In order to allow for low-latency:
        //We receive all available packets without timeout.
        //This call can timeout under regular operation...
        size_t num_samps = _dev->get_device()->recv(
            output_items, noutput_items, _metadata,
            _type, uhd::device::RECV_MODE_FULL_BUFF, 0.0
        );

        //If receive resulted in a timeout condition:
        //We now receive a single packet with a large timeout.
        if (_metadata.error_code == uhd::rx_metadata_t::ERROR_CODE_TIMEOUT){
            num_samps = _dev->get_device()->recv(
                output_items, noutput_items, _metadata,
                _type, uhd::device::RECV_MODE_ONE_PACKET, 1.0
            );
        }

        //handle possible errors conditions
        switch(_metadata.error_code){
        case uhd::rx_metadata_t::ERROR_CODE_NONE:
            //TODO insert tag for time stamp
            break;

        case uhd::rx_metadata_t::ERROR_CODE_TIMEOUT:
            //Assume that the user called stop() on the flow graph.
            //However, a timeout can occur under error conditions.
            return WORK_DONE;

        case uhd::rx_metadata_t::ERROR_CODE_OVERFLOW:
            //ignore overflows and try work again
            //TODO insert tag for overflow
            return work(noutput_items, input_items, output_items);

        default:
            std::cout << boost::format(
                "UHD source block got error code 0x%x"
            ) % _metadata.error_code << std::endl;
            return num_samps;
        }

        return num_samps;
    }

    bool start(void){
        //setup a stream command that starts streaming slightly in the future
        static const double reasonable_delay = 0.1; //order of magnitude over RTT
        uhd::stream_cmd_t stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        stream_cmd.stream_now = _stream_now;
        stream_cmd.time_spec = get_time_now() + uhd::time_spec_t(reasonable_delay);
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
    size_t _nchan;
    bool _stream_now;
    uhd::rx_metadata_t _metadata;
};


/***********************************************************************
 * Make UHD Multi USRP Source
 **********************************************************************/
boost::shared_ptr<uhd_usrp_source> uhd_make_usrp_source(
    const uhd::device_addr_t &device_addr,
    const uhd::io_type_t &io_type,
    size_t num_channels
){
    return boost::shared_ptr<uhd_usrp_source>(
        new uhd_usrp_source_impl(device_addr, io_type, num_channels)
    );
}
