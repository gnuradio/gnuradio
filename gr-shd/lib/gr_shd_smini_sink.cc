/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include <gr_shd_smini_sink.h>
#include <gr_io_signature.h>
#include <stdexcept>

/*********************************************************************
 * SHD Multi SMINI Sink Impl
 ********************************************************************/
class shd_smini_sink_impl : public shd_smini_sink
{
public:
  shd_smini_sink_impl(const shd::device_addr_t &device_addr,
		      const shd::io_type_t &io_type,
		      size_t num_channels):
    gr_sync_block("gr shd smini sink",
		  gr_make_io_signature(num_channels, num_channels, io_type.size),
		  gr_make_io_signature(0, 0, 0)),
    _type(io_type),
    _nchan(num_channels),
    _has_time_spec(_nchan > 1)
  {
    _dev = shd::xmini::multi_xmini::make(device_addr);
  }

  void set_subdev_spec(const std::string &spec,
		       size_t mboard)
  {
    return _dev->set_tx_subdev_spec(spec, mboard);
  }

  void set_samp_rate(double rate){
    _dev->set_tx_rate(rate);
    _sample_rate = this->get_samp_rate();
  }
  
  double get_samp_rate(void){
    return _dev->get_tx_rate();
  }
  
  shd::tune_result_t set_center_freq(
      const shd::tune_request_t tune_request, size_t chan)
  {
    return _dev->set_tx_freq(tune_request, chan);
  }

  double get_center_freq(size_t chan)
  {
    return _dev->get_tx_freq(chan);
  }
  
  shd::freq_range_t get_freq_range(size_t chan)
  {
    return _dev->get_tx_freq_range(chan);
  }
  
  void set_gain(double gain, size_t chan)
  {
    return _dev->set_tx_gain(gain, chan);
  }
  
  void set_gain(double gain, const std::string &name,
		size_t chan)
  {
    return _dev->set_tx_gain(gain, name, chan);
  }
  
  double get_gain(size_t chan){
    return _dev->get_tx_gain(chan);
  }
  
  double get_gain(const std::string &name, size_t chan)
  {
    return _dev->get_tx_gain(name, chan);
  }
  
  std::vector<std::string> get_gain_names(size_t chan)
  {
    return _dev->get_tx_gain_names(chan);
  }
  
  shd::gain_range_t get_gain_range(size_t chan)
  {
    return _dev->get_tx_gain_range(chan);
  }
  
  shd::gain_range_t get_gain_range(const std::string &name,
				   size_t chan)
  {
    return _dev->get_tx_gain_range(name, chan);
  }
  
  void set_antenna(const std::string &ant, size_t chan)
  {
    return _dev->set_tx_antenna(ant, chan);
  }
  
  std::string get_antenna(size_t chan)
  {
    return _dev->get_tx_antenna(chan);
  }
  
  std::vector<std::string> get_antennas(size_t chan)
  {
    return _dev->get_tx_antennas(chan);
  }
  
  void set_bandwidth(double bandwidth, size_t chan)
  {
    return _dev->set_tx_bandwidth(bandwidth, chan);
  }
  
  shd::sensor_value_t get_dboard_sensor(const std::string &name,
					size_t chan)
  {
    return _dev->get_tx_sensor(name, chan);
  }
  
  std::vector<std::string> get_dboard_sensor_names(size_t chan)
  {
    return _dev->get_tx_sensor_names(chan);
  }
  
  shd::sensor_value_t get_mboard_sensor(const std::string &name,
					size_t mboard)
  {
    return _dev->get_mboard_sensor(name, mboard);
  }
  
  std::vector<std::string> get_mboard_sensor_names(size_t mboard)
  {
    return _dev->get_mboard_sensor_names(mboard);
  }
  
  void set_clock_config(const shd::clock_config_t &clock_config,
			size_t mboard)
  {
    return _dev->set_clock_config(clock_config, mboard);
  }
  
  double get_clock_rate(size_t mboard)
  {
    return _dev->get_master_clock_rate(mboard);
  }
  
  void set_clock_rate(double rate, size_t mboard)
  {
    return _dev->set_master_clock_rate(rate, mboard);
  }
  
  shd::time_spec_t get_time_now(size_t mboard = 0)
  {
    return _dev->get_time_now(mboard);
  }
  
  shd::time_spec_t get_time_last_pps(size_t mboard)
  {
    return _dev->get_time_last_pps(mboard);
  }
  
  void set_time_now(const shd::time_spec_t &time_spec,
		    size_t mboard)
  {
    return _dev->set_time_now(time_spec, mboard);
  }
  
  void set_time_next_pps(const shd::time_spec_t &time_spec)
  {
    return _dev->set_time_next_pps(time_spec);
  }
  
  void set_time_unknown_pps(const shd::time_spec_t &time_spec)
  {
    return _dev->set_time_unknown_pps(time_spec);
  }
  
  shd::xmini::dboard_iface::sptr get_dboard_iface(size_t chan)
  {
    return _dev->get_tx_dboard_iface(chan);
  }
  
  shd::xmini::multi_xmini::sptr get_device(void)
  {
    return _dev;
  }
  
  /*******************************************************************
   * Work
   ******************************************************************/
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items)
  {
    //send a mid-burst packet with time spec
    _metadata.start_of_burst = false;
    _metadata.end_of_burst = false;
    _metadata.has_time_spec = _has_time_spec;
    
    size_t num_sent = _dev->get_device()->send(
          input_items, noutput_items, _metadata,
          _type, shd::device::SEND_MODE_FULL_BUFF, 1.0);

    //increment the timespec by the number of samples sent
    _metadata.time_spec += shd::time_spec_t(0, num_sent, _sample_rate);
    return num_sent;
  }
  
  //Send an empty start-of-burst packet to begin streaming.
  //Set at a time in the near future to avoid late packets.
  bool start(void)
  {
    _metadata.start_of_burst = true;
    _metadata.end_of_burst = false;
    _metadata.has_time_spec = _has_time_spec;
    _metadata.time_spec = get_time_now() + shd::time_spec_t(0.01);
    
    _dev->get_device()->send(
          gr_vector_const_void_star(_nchan), 0, _metadata,
          _type, shd::device::SEND_MODE_ONE_PACKET, 1.0);
    return true;
  }
  
  //Send an empty end-of-burst packet to end streaming.
  //Ending the burst avoids an underflow error on stop.
  bool stop(void)
  {
    _metadata.start_of_burst = false;
    _metadata.end_of_burst = true;
    _metadata.has_time_spec = false;
    
    _dev->get_device()->send(
          gr_vector_const_void_star(_nchan), 0, _metadata,
          _type, shd::device::SEND_MODE_ONE_PACKET, 1.0);
    return true;
  }
  
protected:
  shd::xmini::multi_xmini::sptr _dev;
  const shd::io_type_t _type;
  size_t _nchan;
  bool _has_time_spec;
  shd::tx_metadata_t _metadata;
  double _sample_rate;
};

/*********************************************************************
 * Make SHD Multi SMINI Sink
 ********************************************************************/

boost::shared_ptr<shd_smini_sink> shd_make_smini_sink(
    const shd::device_addr_t &device_addr,
    const shd::io_type_t &io_type,
    size_t num_channels)
{
  return boost::shared_ptr<shd_smini_sink>(
        new shd_smini_sink_impl(device_addr, io_type, num_channels));
}
