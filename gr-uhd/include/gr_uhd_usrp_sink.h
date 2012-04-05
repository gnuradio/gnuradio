/*
 * Copyright 2010-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_UHD_USRP_SINK_H
#define INCLUDED_GR_UHD_USRP_SINK_H

#include <gr_uhd_api.h>
#include <gr_sync_block.h>
#include <uhd/usrp/multi_usrp.hpp>

#ifndef INCLUDED_UHD_STREAM_HPP
namespace uhd{
    struct GR_UHD_API stream_args_t{
        stream_args_t(
            const std::string &cpu = "",
            const std::string &otw = ""
        ){
            cpu_format = cpu;
            otw_format = otw;
        }
        std::string cpu_format;
        std::string otw_format;
        device_addr_t args;
        std::vector<size_t> channels;
    };
}
#  define INCLUDED_UHD_STREAM_HPP
#else
#  define GR_UHD_USE_STREAM_API
#endif

class uhd_usrp_sink;

/*!
 * \brief Make a new USRP sink block.
 * \ingroup uhd_blk
 *
 * The USRP sink block reads a stream and transmits the samples.
 * The sink block also provides API calls for transmitter settings.
 *
 * TX Stream tagging:
 *
 * The following tag keys will be consumed by the work function:
 *  - pmt::pmt_string_to_symbol("tx_sob")
 *  - pmt::pmt_string_to_symbol("tx_eob")
 *  - pmt::pmt_string_to_symbol("tx_time")
 *
 * The sob and eob (start and end of burst) tag values are pmt booleans.
 * When present, burst tags should be set to true (pmt::PMT_T).
 *
 * The timstamp tag value is a pmt tuple of the following:
 * (uint64 seconds, and double fractional seconds).
 *
 * See the UHD manual for more detailed documentation:
 * http://code.ettus.com/redmine/ettus/projects/uhd/wiki
 *
 * \param device_addr the address to identify the hardware
 * \param io_type the desired input data type
 * \param num_channels number of stream from the device
 * \return a new USRP sink block object
 */
GR_UHD_API boost::shared_ptr<uhd_usrp_sink> uhd_make_usrp_sink(
    const uhd::device_addr_t &device_addr,
    const uhd::io_type_t &io_type,
    size_t num_channels
);

/*!
 * \brief Make a new USRP sink block.
 *
 * The USRP sink block reads a stream and transmits the samples.
 * The sink block also provides API calls for transmitter settings.
 *
 * TX Stream tagging:
 *
 * The following tag keys will be consumed by the work function:
 *  - pmt::pmt_string_to_symbol("tx_sob")
 *  - pmt::pmt_string_to_symbol("tx_eob")
 *  - pmt::pmt_string_to_symbol("tx_time")
 *
 * The sob and eob (start and end of burst) tag values are pmt booleans.
 * When present, burst tags should be set to true (pmt::PMT_T).
 *
 * The timstamp tag value is a pmt tuple of the following:
 * (uint64 seconds, and double fractional seconds).
 *
 * See the UHD manual for more detailed documentation:
 * http://code.ettus.com/redmine/ettus/projects/uhd/wiki
 *
 * \param device_addr the address to identify the hardware
 * \param stream_args the IO format and channel specification
 * \return a new USRP sink block object
 */
GR_UHD_API boost::shared_ptr<uhd_usrp_sink> uhd_make_usrp_sink(
    const uhd::device_addr_t &device_addr,
    const uhd::stream_args_t &stream_args
);

class GR_UHD_API uhd_usrp_sink : virtual public gr_sync_block{
public:

    /*!
     * Set the start time for outgoing samples.
     * To control when samples are transmitted,
     * set this value before starting the flow graph.
     * The value is cleared after each run.
     * When not specified, the start time will be:
     *  - Immediately for the one channel case
     *  - in the near future for multi-channel
     *
     * \param time the absolute time for transmission to begin
     */
    virtual void set_start_time(const uhd::time_spec_t &time) = 0;

    /*!
     * Returns identifying information about this USRP's configuration.
     * Returns motherboard ID, name, and serial.
     * Returns daughterboard TX ID, subdev name, and serial.
     * \param chan channel index 0 to N-1
     * \return TX info
     */
    virtual uhd::dict<std::string, std::string> get_usrp_info(size_t chan = 0) = 0;

    /*!
     * Set the frontend specification.
     * \param spec the subdev spec markup string
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_subdev_spec(const std::string &spec, size_t mboard = 0) = 0;


     /*!
     * Get the TX frontend specification.
     * \param mboard the motherboard index 0 to M-1
     * \return the frontend specification in use
     */
    virtual std::string get_subdev_spec (size_t mboard = 0) = 0;

    /*!
     * Set the sample rate for the usrp device.
     * \param rate a new rate in Sps
     */
    virtual void set_samp_rate(double rate) = 0;

    /*!
     * Get the sample rate for the usrp device.
     * This is the actual sample rate and may differ from the rate set.
     * \return the actual rate in Sps
     */
    virtual double get_samp_rate(void) = 0;

    /*!
     * Get the possible sample rates for the usrp device.
     * \return a range of rates in Sps
     */
    virtual uhd::meta_range_t get_samp_rates(void) = 0;

    /*!
     * Tune the usrp device to the desired center frequency.
     * \param tune_request the tune request instructions
     * \param chan the channel index 0 to N-1
     * \return a tune result with the actual frequencies
     */
    virtual uhd::tune_result_t set_center_freq(
        const uhd::tune_request_t tune_request, size_t chan = 0
    ) = 0;

    /*!
     * Tune the usrp device to the desired center frequency.
     * This is a wrapper around set center freq so that in this case,
     * the user can pass a single frequency in the call through swig.
     * \param freq the desired frequency in Hz
     * \param chan the channel index 0 to N-1
     * \return a tune result with the actual frequencies
     */
    uhd::tune_result_t set_center_freq(double freq, size_t chan = 0){
        return set_center_freq(uhd::tune_request_t(freq), chan);
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
    virtual uhd::freq_range_t get_freq_range(size_t chan = 0) = 0;

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
    virtual void set_gain(double gain, const std::string &name, size_t chan = 0) = 0;

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
    virtual double get_gain(const std::string &name, size_t chan = 0) = 0;

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
    virtual uhd::gain_range_t get_gain_range(size_t chan = 0) = 0;

    /*!
     * Get the settable gain range.
     * \param name the name of the gain stage
     * \param chan the channel index 0 to N-1
     * \return the gain range in dB
     */
    virtual uhd::gain_range_t get_gain_range(const std::string &name, size_t chan = 0) = 0;

    /*!
     * Set the antenna to use.
     * \param ant the antenna string
     * \param chan the channel index 0 to N-1
     */
    virtual void set_antenna(const std::string &ant, size_t chan = 0) = 0;

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
     * Set the bandpass filter on the RF frontend.
     * \param chan the channel index 0 to N-1
     * \param bandwidth the filter bandwidth in Hz
     */
    virtual void set_bandwidth(double bandwidth, size_t chan = 0) = 0;

    /*!
     * Set a constant DC offset value.
     * The value is complex to control both I and Q.
     * \param offset the dc offset (1.0 is full-scale)
     * \param chan the channel index 0 to N-1
     */
    virtual void set_dc_offset(const std::complex<double> &offset, size_t chan = 0) = 0;

    /*!
     * Set the RX frontend IQ imbalance correction.
     * Use this to adjust the magnitude and phase of I and Q.
     *
     * \param correction the complex correction (1.0 is full-scale)
     * \param chan the channel index 0 to N-1
     */
    virtual void set_iq_balance(const std::complex<double> &correction, size_t chan = 0) = 0;

    /*!
     * Get an RF frontend sensor value.
     * \param name the name of the sensor
     * \param chan the channel index 0 to N-1
     * \return a sensor value object
     */
    virtual uhd::sensor_value_t get_sensor(const std::string &name, size_t chan = 0) = 0;

    /*!
     * Get a list of possible RF frontend sensor names.
     * \param chan the channel index 0 to N-1
     * \return a vector of sensor names
     */
    virtual std::vector<std::string> get_sensor_names(size_t chan = 0) = 0;

    //! DEPRECATED use get_sensor
    uhd::sensor_value_t get_dboard_sensor(const std::string &name, size_t chan = 0){
        return this->get_sensor(name, chan);
    }

    //! DEPRECATED use get_sensor_names
    std::vector<std::string> get_dboard_sensor_names(size_t chan = 0){
        return this->get_sensor_names(chan);
    }

    /*!
     * Get a motherboard sensor value.
     * \param name the name of the sensor
     * \param mboard the motherboard index 0 to M-1
     * \return a sensor value object
     */
    virtual uhd::sensor_value_t get_mboard_sensor(const std::string &name, size_t mboard = 0) = 0;

    /*!
     * Get a list of possible motherboard sensor names.
     * \param mboard the motherboard index 0 to M-1
     * \return a vector of sensor names
     */
    virtual std::vector<std::string> get_mboard_sensor_names(size_t mboard = 0) = 0;

    /*!
     * Set the clock configuration.
     * DEPRECATED for set_time/clock_source.
     * \param clock_config the new configuration
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_clock_config(const uhd::clock_config_t &clock_config, size_t mboard = 0) = 0;

    /*!
     * Set the time source for the usrp device.
     * This sets the method of time synchronization,
     * typically a pulse per second or an encoded time.
     * Typical options for source: external, MIMO.
     * \param source a string representing the time source
     * \param mboard which motherboard to set the config
     */
    virtual void set_time_source(const std::string &source, const size_t mboard = 0) = 0;

    /*!
     * Get the currently set time source.
     * \param mboard which motherboard to get the config
     * \return the string representing the time source
     */
    virtual std::string get_time_source(const size_t mboard) = 0;

    /*!
     * Get a list of possible time sources.
     * \param mboard which motherboard to get the list
     * \return a vector of strings for possible settings
     */
    virtual std::vector<std::string> get_time_sources(const size_t mboard) = 0;

    /*!
     * Set the clock source for the usrp device.
     * This sets the source for a 10 Mhz reference clock.
     * Typical options for source: internal, external, MIMO.
     * \param source a string representing the clock source
     * \param mboard which motherboard to set the config
     */
    virtual void set_clock_source(const std::string &source, const size_t mboard = 0) = 0;

    /*!
     * Get the currently set clock source.
     * \param mboard which motherboard to get the config
     * \return the string representing the clock source
     */
    virtual std::string get_clock_source(const size_t mboard) = 0;

    /*!
     * Get a list of possible clock sources.
     * \param mboard which motherboard to get the list
     * \return a vector of strings for possible settings
     */
    virtual std::vector<std::string> get_clock_sources(const size_t mboard) = 0;

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
    virtual void set_clock_rate(double rate, size_t mboard = 0) = 0;

    /*!
     * Get the current time registers.
     * \param mboard the motherboard index 0 to M-1
     * \return the current usrp time
     */
    virtual uhd::time_spec_t get_time_now(size_t mboard = 0) = 0;

    /*!
     * Get the time when the last pps pulse occured.
     * \param mboard the motherboard index 0 to M-1
     * \return the current usrp time
     */
    virtual uhd::time_spec_t get_time_last_pps(size_t mboard = 0) = 0;

    /*!
     * Sets the time registers immediately.
     * \param time_spec the new time
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_time_now(const uhd::time_spec_t &time_spec, size_t mboard = 0) = 0;

    /*!
     * Set the time registers at the next pps.
     * \param time_spec the new time
     */
    virtual void set_time_next_pps(const uhd::time_spec_t &time_spec) = 0;

    /*!
     * Sync the time registers with an unknown pps edge.
     * \param time_spec the new time
     */
    virtual void set_time_unknown_pps(const uhd::time_spec_t &time_spec) = 0;

    /*!
     * Set the time at which the control commands will take effect.
     *
     * A timed command will back-pressure all subsequent timed commands,
     * assuming that the subsequent commands occur within the time-window.
     * If the time spec is late, the command will be activated upon arrival.
     *
     * \param time_spec the time at which the next command will activate
     * \param mboard which motherboard to set the config
     */
    virtual void set_command_time(const uhd::time_spec_t &time_spec, size_t mboard = 0) = 0;

    /*!
     * Clear the command time so future commands are sent ASAP.
     *
     * \param mboard which motherboard to set the config
     */
    virtual void clear_command_time(size_t mboard = 0) = 0;

    /*!
     * Get access to the underlying uhd dboard iface object.
     * \return the dboard_iface object
     */
    virtual uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan = 0) = 0;

    /*!
     * Get access to the underlying uhd device object.
     * \return the multi usrp device object
     */
    virtual uhd::usrp::multi_usrp::sptr get_device(void) = 0;

    /*!
     * Perform write on the user configuration register bus.  These only exist if
     * the user has implemented custom setting registers in the device FPGA.
     * \param addr 8-bit register address
     * \param data 32-bit register value
     * \param mboard which motherboard to set the user register
     */
    virtual void set_user_register(const uint8_t addr, const uint32_t data, size_t mboard = 0) = 0;
};

#endif /* INCLUDED_GR_UHD_USRP_SINK_H */
