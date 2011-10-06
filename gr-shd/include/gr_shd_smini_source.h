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

#ifndef INCLUDED_GR_SHD_SMINI_SOURCE_H
#define INCLUDED_GR_SHD_SMINI_SOURCE_H

#include <gr_shd_api.h>
#include <gr_sync_block.h>
#include <shd/xmini/multi_xmini.hpp>

class shd_smini_source;

GR_SHD_API boost::shared_ptr<shd_smini_source> shd_make_smini_source(
    const shd::device_addr_t &device_addr,
    const shd::io_type_t &io_type,
    size_t num_channels
);

class GR_SHD_API shd_smini_source : virtual public gr_sync_block
{
 public:
  
  /*!
   * Set the subdevice specification.
   * \param spec the subdev spec markup string
   * \param mboard the motherboard index 0 to M-1
   */
  virtual void set_subdev_spec(const std::string &spec,
			       size_t mboard = 0) = 0;
  
  /*!
   * Set the sample rate for the smini device.
   * \param rate a new rate in Sps
   */
  virtual void set_samp_rate(double rate) = 0;

  /*!
   * Get the sample rate for the smini device.
   * This is the actual sample rate and may differ from the rate set.
   * \return the actual rate in Sps
   */
  virtual double get_samp_rate(void) = 0;
  
  /*!
   * Tune the smini device to the desired center frequency.
   * \param tune_request the tune request instructions
   * \param chan the channel index 0 to N-1
   * \return a tune result with the actual frequencies
   */
  virtual shd::tune_result_t set_center_freq(
        const shd::tune_request_t tune_request,
	size_t chan = 0
    ) = 0;
  
  /*!
   * Tune the smini device to the desired center frequency.
   * This is a wrapper around set center freq so that in this case,
   * the user can pass a single frequency in the call through swig.
   * \param freq the desired frequency in Hz
   * \param chan the channel index 0 to N-1
   * \return a tune result with the actual frequencies
   */
  shd::tune_result_t set_center_freq(double freq, size_t chan = 0){
    return set_center_freq(shd::tune_request_t(freq), chan);
  }
  
  /*!
   * Get the center frequency.
   * \param chan the channel index 0 to N-1
   * \return the frequency in Hz
   */
  virtual double get_center_freq(size_t chan = 0) = 0;
  
  /*!
   * Get the tunable frequency range.
   * \param chan the channel index 0 to N-1
   * \return the frequency range in Hz
   */
  virtual shd::freq_range_t get_freq_range(size_t chan = 0) = 0;
  
  /*!
   * Set the gain for the dboard.
   * \param gain the gain in dB
   * \param chan the channel index 0 to N-1
   */
  virtual void set_gain(double gain, size_t chan = 0) = 0;
  
  /*!
   * Set the named gain on the dboard.
   * \param gain the gain in dB
   * \param name the name of the gain stage
   * \param chan the channel index 0 to N-1
   */
  virtual void set_gain(double gain, const std::string &name,
			size_t chan = 0) = 0;
  
  /*!
   * Get the actual dboard gain setting.
   * \param chan the channel index 0 to N-1
   * \return the actual gain in dB
   */
  virtual double get_gain(size_t chan = 0) = 0;
  
  /*!
   * Get the actual dboard gain setting of named stage.
   * \param name the name of the gain stage
   * \param chan the channel index 0 to N-1
   * \return the actual gain in dB
   */
  virtual double get_gain(const std::string &name,
			  size_t chan = 0) = 0;
  
  /*!
   * Get the actual dboard gain setting of named stage.
   * \param chan the channel index 0 to N-1
   * \return the actual gain in dB
   */
  virtual std::vector<std::string> get_gain_names(size_t chan = 0) = 0;
  
  /*!
   * Get the settable gain range.
   * \param chan the channel index 0 to N-1
   * \return the gain range in dB
   */
  virtual shd::gain_range_t get_gain_range(size_t chan = 0) = 0;
  
  /*!
   * Get the settable gain range.
   * \param name the name of the gain stage
   * \param chan the channel index 0 to N-1
   * \return the gain range in dB
   */
  virtual shd::gain_range_t get_gain_range(const std::string &name,
					   size_t chan = 0) = 0;
  
  /*!
   * Set the antenna to use.
   * \param ant the antenna string
   * \param chan the channel index 0 to N-1
   */
  virtual void set_antenna(const std::string &ant,
			   size_t chan = 0) = 0;
  
  /*!
   * Get the antenna in use.
   * \param chan the channel index 0 to N-1
   * \return the antenna string
   */
  virtual std::string get_antenna(size_t chan = 0) = 0;
  
  /*!
   * Get a list of possible antennas.
   * \param chan the channel index 0 to N-1
   * \return a vector of antenna strings
   */
  virtual std::vector<std::string> get_antennas(size_t chan = 0) = 0;
  
  /*!
   * Set the subdevice bandpass filter.
   * \param bandwidth the filter bandwidth in Hz
   * \param chan the channel index 0 to N-1
   */
  virtual void set_bandwidth(double bandwidth,
			     size_t chan = 0) = 0;
  
  /*!
   * Get a daughterboard sensor value.
   * \param name the name of the sensor
   * \param chan the channel index 0 to N-1
   * \return a sensor value object
   */
  virtual shd::sensor_value_t get_dboard_sensor(const std::string &name,
						size_t chan = 0) = 0;
  
  /*!
   * Get a list of possible daughterboard sensor names.
   * \param chan the channel index 0 to N-1
   * \return a vector of sensor names
   */
  virtual std::vector<std::string> get_dboard_sensor_names(size_t chan = 0) = 0;
  
  /*!
   * Get a motherboard sensor value.
   * \param name the name of the sensor
   * \param mboard the motherboard index 0 to M-1
   * \return a sensor value object
   */
  virtual shd::sensor_value_t get_mboard_sensor(const std::string &name,
						size_t mboard = 0) = 0;
  
  /*!
   * Get a list of possible motherboard sensor names.
   * \param mboard the motherboard index 0 to M-1
   * \return a vector of sensor names
   */
  virtual std::vector<std::string> get_mboard_sensor_names(size_t mboard = 0) = 0;
  
  /*!
   * Set the clock configuration.
   * \param clock_config the new configuration
   * \param mboard the motherboard index 0 to M-1
   */
  virtual void set_clock_config(const shd::clock_config_t &clock_config,
				size_t mboard = 0) = 0;
  
  /*!
   * Get the master clock rate.
   * \param mboard the motherboard index 0 to M-1
   * \return the clock rate in Hz
   */
  virtual double get_clock_rate(size_t mboard = 0) = 0;
  
  /*!
   * Set the master clock rate.
   * \param rate the new rate in Hz
   * \param mboard the motherboard index 0 to M-1
   */
  virtual void set_clock_rate(double rate,
			      size_t mboard = 0) = 0;

  /*!
   * Get the current time registers.
   * \param mboard the motherboard index 0 to M-1
   * \return the current smini time
   */
  virtual shd::time_spec_t get_time_now(size_t mboard = 0) = 0;
  
  /*!
   * Get the time when the last pps pulse occured.
   * \param mboard the motherboard index 0 to M-1
   * \return the current smini time
   */
  virtual shd::time_spec_t get_time_last_pps(size_t mboard = 0) = 0;
  
  /*!
   * Sets the time registers immediately.
   * \param time_spec the new time
   * \param mboard the motherboard index 0 to M-1
   */
  virtual void set_time_now(const shd::time_spec_t &time_spec,
			    size_t mboard = 0) = 0;

  /*!
   * Set the time registers at the next pps.
   * \param time_spec the new time
   */
  virtual void set_time_next_pps(const shd::time_spec_t &time_spec) = 0;
  
  /*!
   * Sync the time registers with an unknown pps edge.
   * \param time_spec the new time
   */
  virtual void set_time_unknown_pps(const shd::time_spec_t &time_spec) = 0;
  
  /*!
   * Get access to the underlying shd dboard iface object.
   * \return the dboard_iface object
   */
  virtual shd::xmini::dboard_iface::sptr get_dboard_iface(size_t chan = 0) = 0;
  
  /*!
   * Get access to the underlying shd device object.
   * \return the multi smini device object
   */
  virtual shd::xmini::multi_xmini::sptr get_device(void) = 0;
};

#endif /* INCLUDED_GR_SHD_SMINI_SOURCE_H */
