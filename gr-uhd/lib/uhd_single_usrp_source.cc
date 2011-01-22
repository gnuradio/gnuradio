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

#include <uhd_single_usrp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>

/***********************************************************************
 * UHD Single USRP Source
 **********************************************************************/
uhd_single_usrp_source::uhd_single_usrp_source(gr_io_signature_sptr sig)
:gr_sync_block("uhd single_usrp source", gr_make_io_signature(0, 0, 0), sig){
    /* NOP */
}

/***********************************************************************
 * UHD Single USRP Source Impl
 **********************************************************************/
class uhd_single_usrp_source_impl : public uhd_single_usrp_source{
public:
    uhd_single_usrp_source_impl(
        const uhd::device_addr_t &device_addr,
        const uhd::io_type_t &io_type,
        size_t num_channels
    ):
        uhd_single_usrp_source(gr_make_io_signature(
            num_channels, num_channels, io_type.size
        )),
        _type(io_type)
    {
        _dev = uhd::usrp::single_usrp::make(device_addr);
    }

    void set_subdev_spec(const std::string &spec){
        return _dev->set_rx_subdev_spec(spec);
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

    uhd::freq_range_t get_freq_range(size_t chan){
        return _dev->get_rx_freq_range(chan);
    }

    void set_gain(double gain, size_t chan){
        return _dev->set_rx_gain(gain, chan);
    }

    double get_gain(size_t chan){
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

    void set_bandwidth(double bandwidth, size_t chan){
        return _dev->set_rx_bandwidth(bandwidth, chan);
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

    uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan){
        return _dev->get_rx_dboard_iface(chan);
    }

    uhd::usrp::single_usrp::sptr get_device(void){
        return _dev;
    }

/***********************************************************************
 * Work
 **********************************************************************/
    virtual int work(
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
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        return true;
    }

    bool stop(void){
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        return true;
    }

protected:
    uhd::usrp::single_usrp::sptr _dev;
    const uhd::io_type_t _type;
};

/***********************************************************************
 * UHD Single USRP Source Impl with Tags
 **********************************************************************/
class uhd_single_usrp_source_impl_with_tags : public uhd_single_usrp_source_impl{
public:
    uhd_single_usrp_source_impl_with_tags(
        const uhd::device_addr_t &device_addr,
        const uhd::io_type_t &io_type,
        size_t num_channels
    ):
        uhd_single_usrp_source_impl(device_addr, io_type, num_channels)
    {
        d_tag_srcid = pmt::mp("uhd_single_usrp_source");
    }

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
        //FIXME in RECV_MODE_FULL_BUFF we are probably get remainders,
        //there is no guarantee of has_time_spec, its random
	  if(metadata.has_time_spec) {
	    d_tstamp_pair = pmt::mp(pmt::mp(metadata.time_spec.get_full_secs()),
				    pmt::mp(metadata.time_spec.get_frac_secs()));
	    add_item_tag(0, nitems_written(0),
			 //gr_tags::key_time,
			 pmt::pmt_string_to_symbol("time"),
			 d_tstamp_pair,
			 d_tag_srcid);
	  }
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

private:
    size_t d_num_packet_samps;
    pmt::pmt_t d_tstamp_pair;
    pmt::pmt_t d_tag_srcid;
};


/***********************************************************************
 * Make UHD Single USRP Source
 **********************************************************************/
#include <gr_prefs.h>
boost::shared_ptr<uhd_single_usrp_source> uhd_make_single_usrp_source(
    const uhd::device_addr_t &device_addr,
    const uhd::io_type_t &io_type,
    size_t num_channels
){
    //use the tags constructor if tags is set to on
    bool tags_enabled = (gr_prefs().get_string("gr-uhd", "tags", "off") == "on");
    if (tags_enabled) return boost::shared_ptr<uhd_single_usrp_source>(
        new uhd_single_usrp_source_impl_with_tags(device_addr, io_type, num_channels)
    );

    return boost::shared_ptr<uhd_single_usrp_source>(
        new uhd_single_usrp_source_impl(device_addr, io_type, num_channels)
    );
}
