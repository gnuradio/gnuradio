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

#include <uhd_single_usrp_source.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <boost/format.hpp>
#include <gr_tag_info.h>

/***********************************************************************
 * UHD Single USRP Source
 **********************************************************************/
uhd_single_usrp_source::uhd_single_usrp_source(gr_io_signature_sptr sig)
 :gr_sync_block("uhd single_usrp source", gr_make_io_signature(0, 0, 0), sig)
{
  d_num_packet_samps = 0;
  d_tstamp_pair = pmt::mp(pmt::mp(0), pmt::mp(0));
}

/***********************************************************************
 * UHD Single USRP Source Impl
 **********************************************************************/
class uhd_single_usrp_source_impl : public uhd_single_usrp_source{
public:
    uhd_single_usrp_source_impl(
        const std::string &device_addr,
        const uhd::io_type_t &io_type,
        size_t num_channels
    ):
        uhd_single_usrp_source(gr_make_io_signature(
            num_channels, num_channels, io_type.size
        )),
        _type(io_type)
    {
        _dev = uhd::usrp::single_usrp::make(device_addr);

	d_tag_srcid = pmt::mp("uhd_single_usrp_source");
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

    uhd::usrp::single_usrp::sptr get_device(void){
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

	size_t total_samps = 0;
	while(total_samps + 362 < (size_t)noutput_items) {
	  size_t num_samps = _dev->get_device()->recv(
              output_items, noutput_items, metadata,
	      //_type, uhd::device::RECV_MODE_FULL_BUFF
              _type, uhd::device::RECV_MODE_ONE_PACKET
          );
	  total_samps += num_samps;

	  switch(metadata.error_code){
	  case uhd::rx_metadata_t::ERROR_CODE_NONE:
	    //keep track of the number of accumulated samples in this packet
	    if (metadata.fragment_offset == 0) {
	      d_num_packet_samps = 0;
	      d_tstamp_pair = pmt::mp(pmt::mp(metadata.time_spec.get_full_secs()),
				      pmt::mp(metadata.time_spec.get_frac_secs()));
	    }
	    d_num_packet_samps += num_samps;
	    
	    //don't push on to the queue until we get the final fragment
	    if (!metadata.more_fragments) {
	      // Create tags with time and num samples
	      pmt::pmt_t tsamp, nsamp;
	      pmt::pmt_t nsamp_val = pmt::mp((int)d_num_packet_samps);
	      
	      add_item_tag(0, nitems_written(0),
			   gr_tags::key_time,
			   d_tstamp_pair,
			   d_tag_srcid);
	      add_item_tag(0, nitems_written(0),
			   pmt::mp("num_packet_samples"),
			   nsamp_val,
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
	return -1;
    }

    bool start(void){
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_START_CONTINUOUS);
        return true;
    }

    bool stop(void){
        _dev->issue_stream_cmd(uhd::stream_cmd_t::STREAM_MODE_STOP_CONTINUOUS);
        return true;
    }

private:
    uhd::usrp::single_usrp::sptr _dev;
    const uhd::io_type_t _type;
    pmt::pmt_t d_tag_srcid;
};


/***********************************************************************
 * Make UHD Single USRP Source
 **********************************************************************/
boost::shared_ptr<uhd_single_usrp_source> uhd_make_single_usrp_source(
    const std::string &device_addr,
    const uhd::io_type_t::tid_t &io_type,
    size_t num_channels
){
    return boost::shared_ptr<uhd_single_usrp_source>(
        new uhd_single_usrp_source_impl(device_addr, io_type, num_channels)
    );
}
