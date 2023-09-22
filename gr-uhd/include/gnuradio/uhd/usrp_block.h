/* -*- c++ -*- */
/*
 * Copyright 2015,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UHD_USRP_BLOCK_H
#define INCLUDED_GR_UHD_USRP_BLOCK_H

#include <gnuradio/sync_block.h>
#include <gnuradio/uhd/api.h>
#include <uhd/usrp/multi_usrp.hpp>
#include <cstdint>

namespace gr {
namespace uhd {

GR_UHD_API const pmt::pmt_t cmd_chan_key();
GR_UHD_API const pmt::pmt_t cmd_gain_key();
GR_UHD_API const pmt::pmt_t cmd_power_key();
GR_UHD_API const pmt::pmt_t cmd_freq_key();
GR_UHD_API const pmt::pmt_t cmd_lo_offset_key();
GR_UHD_API const pmt::pmt_t cmd_tune_key();
GR_UHD_API const pmt::pmt_t cmd_mtune_key();
GR_UHD_API const pmt::pmt_t cmd_lo_freq_key();
GR_UHD_API const pmt::pmt_t cmd_dsp_freq_key();
GR_UHD_API const pmt::pmt_t cmd_rate_key();
GR_UHD_API const pmt::pmt_t cmd_bandwidth_key();
GR_UHD_API const pmt::pmt_t cmd_time_key();
GR_UHD_API const pmt::pmt_t cmd_mboard_key();
GR_UHD_API const pmt::pmt_t cmd_antenna_key();
GR_UHD_API const pmt::pmt_t cmd_direction_key();
GR_UHD_API const pmt::pmt_t cmd_tag_key();
GR_UHD_API const pmt::pmt_t cmd_gpio_key();
GR_UHD_API const pmt::pmt_t cmd_pc_clock_resync_key();

GR_UHD_API const pmt::pmt_t direction_rx();
GR_UHD_API const pmt::pmt_t direction_tx();

/*! Base class for USRP blocks.
 * \ingroup uhd_blk
 *
 * Note that many of the functions defined here differ between
 * Rx and Tx configurations. As an example, set_center_freq()
 * will set the Rx frequency for a usrp_source object, and the
 * Tx frequency on a usrp_sink object.
 */
class GR_UHD_API usrp_block : public gr::sync_block
{
protected:
    usrp_block(){}; // For virtual sub-classing
    usrp_block(const std::string& name,
               gr::io_signature::sptr input_signature,
               gr::io_signature::sptr output_signature);

public:
    /*!
     * Set the frontend specification.
     *
     * \param spec the subdev spec markup string
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_subdev_spec(const std::string& spec, size_t mboard = 0) = 0;

    /*!
     * Get the frontend specification.
     *
     * \param mboard the motherboard index 0 to M-1
     * \return the frontend specification in use
     */
    virtual std::string get_subdev_spec(size_t mboard = 0) = 0;

    /*!
     * Return the number of motherboards in this configuration.
     */
    virtual size_t get_num_mboards() = 0;

    /*!
     * Set the sample rate for this connection to the USRP.
     *
     * \param rate a new rate in Sps
     */
    virtual void set_samp_rate(double rate) = 0;

    /*!
     * Get the sample rate for this connection to the USRP.
     * This is the actual sample rate and may differ from the rate set.
     *
     * \return the actual rate in Sps
     */
    virtual double get_samp_rate(void) = 0;

    /*!
     * Get the possible sample rates for this connection.
     *
     * \return a range of rates in Sps
     */
    virtual ::uhd::meta_range_t get_samp_rates(void) = 0;

    /*!
     * Tune the selected channel to the desired center frequency.
     *
     * \param tune_request the tune request instructions
     * \param chan the channel index 0 to N-1
     * \return a tune result with the actual frequencies
     */
    virtual ::uhd::tune_result_t set_center_freq(const ::uhd::tune_request_t tune_request,
                                                 size_t chan = 0) = 0;

    /*!
     * Tune the selected channel to the desired center frequency.
     *
     * This is a wrapper around set_center_freq() so that in this case,
     * the user can pass a single frequency in the call instead of
     * having to generate a tune_request_t object.
     *
     * \param freq the desired frequency in Hz
     * \param chan the channel index 0 to N-1
     * \return a tune result with the actual frequencies
     */
    ::uhd::tune_result_t set_center_freq(double freq, size_t chan = 0)
    {
        return set_center_freq(::uhd::tune_request_t(freq), chan);
    }

    /*!
     * Get the center frequency.
     *
     * \param chan the channel index 0 to N-1
     * \return the frequency in Hz
     */
    virtual double get_center_freq(size_t chan = 0) = 0;

    /*!
     * Get the tunable frequency range.
     *
     * \param chan the channel index 0 to N-1
     * \return the frequency range in Hz
     */
    virtual ::uhd::freq_range_t get_freq_range(size_t chan = 0) = 0;

    /*!
     * Set the gain for the selected channel.
     *
     * \param gain the gain in dB
     * \param chan the channel index 0 to N-1
     * \param direction TX or RX. This is mostly used by the internal message
     *        handling.
     */
    virtual void
    set_gain(double gain, size_t chan = 0, pmt::pmt_t direction = pmt::PMT_NIL) = 0;

    /*!
     * Set the named gain on the dboard.
     *
     * \param gain the gain in dB
     * \param name the name of the gain stage
     * \param chan the channel index 0 to N-1
     */
    virtual void set_gain(double gain, const std::string& name, size_t chan = 0) = 0;

    /*!
     * Set the normalized gain.
     *
     * The normalized gain is always in [0, 1], regardless of the device.
     * 0 corresponds to minimum gain (usually 0 dB, but make sure to read the device
     * notes in the UHD manual) and 1 corresponds to maximum gain.
     * This will work for any UHD device. Use get_gain() to see which dB value
     * the normalized gain value corresponds to.
     *
     * Note that it is not possible to specify a gain name for this function.
     *
     * \throws A runtime_error if \p norm_gain is not within the valid range.
     *
     * \param norm_gain the gain in fractions of the gain range (must be 0 <= norm_gain <=
     * 1) \param chan the channel index 0 to N-1
     */
    virtual void set_normalized_gain(double norm_gain, size_t chan = 0) = 0;

    /*!
     * Get the actual dboard gain setting.
     *
     * \param chan the channel index 0 to N-1
     * \return the actual gain in dB
     */
    virtual double get_gain(size_t chan = 0) = 0;

    /*!
     * Get the actual dboard gain setting of named stage.
     *
     * \param name the name of the gain stage
     * \param chan the channel index 0 to N-1
     * \return the actual gain in dB
     */
    virtual double get_gain(const std::string& name, size_t chan = 0) = 0;

    /*!
     * Returns the normalized gain.
     *
     * The normalized gain is always in [0, 1], regardless of the device.
     * See also set_normalized_gain().
     *
     * Note that it is not possible to specify a gain name for
     * this function, the result is over the entire gain chain.
     *
     * \param chan the channel index 0 to N-1
     */
    virtual double get_normalized_gain(size_t chan = 0) = 0;

    /*!
     * Get the actual dboard gain setting of named stage.
     *
     * \param chan the channel index 0 to N-1
     * \return the actual gain in dB
     */
    virtual std::vector<std::string> get_gain_names(size_t chan = 0) = 0;

    /*!
     * Get the settable gain range.
     *
     * \param chan the channel index 0 to N-1
     * \return the gain range in dB
     */
    virtual ::uhd::gain_range_t get_gain_range(size_t chan = 0) = 0;

    /*!
     * Get the settable gain range.
     *
     * \param name the name of the gain stage
     * \param chan the channel index 0 to N-1
     * \return the gain range in dB
     */
    virtual ::uhd::gain_range_t get_gain_range(const std::string& name,
                                               size_t chan = 0) = 0;

    /*! Query if this device is capable of absolute power levels
     *
     * If true, the set_power_reference() and get_power_reference() APIs can be
     * used as well.
     * Note that if the underlying UHD version doesn't support power APIs, a
     * warning will be printed, and the return value is always false.
     *
     * \param chan the channel index 0 to N-1
     * \returns true if there is a power reference API available for this channel
     */
    virtual bool has_power_reference(size_t chan = 0) = 0;

    /*! Set the absolute power reference level for this channel
     *
     * Note that this API is available for certain devices only, and only if
     * calibration data is available. Refer to the UHD manual for greater
     * detail: https://files.ettus.com/manual/page_power.html
     *
     * In a nutshell, using the power reference will configure the device such
     * that a full-scale signal (0 dBFS) corresponds to a signal at the
     * antenna connector of \p power_dbm.
     * After calling this function, the device will attempt to keep the power
     * level constant after retuning, which means the gain level may be changed
     * after a re-tune.
     *
     * The device may coerce the available power level (for example, if the
     * requested power level is not achievable by the device). The coerced
     * value may be read by calling get_power_reference().
     *
     * \param power_dbm The power reference level in dBm
     * \param chan the channel index 0 to N-1
     * \throws std::runtime_error if the underlying UHD version does not support
     *         the power API.
     */
    virtual void set_power_reference(double power_dbm, size_t chan = 0) = 0;

    /*! Return the absolute power reference level for this channel
     *
     * Note that this API is only available for certain devices, and assuming
     * the existence of calibration data. Refer to the UHD manual for greater
     * detail: https://files.ettus.com/manual/page_compat.html
     *
     * See also set_power_reference().
     *
     * \param chan the channel index 0 to N-1
     * \throws std::runtime_error if the underlying UHD version does not support
     *         the power API.
     */
    virtual double get_power_reference(size_t chan = 0) = 0;

    /*! Return the available power range
     *
     * \param chan the channel index 0 to N-1
     * \return the power range in dBm
     * \throws std::runtime_error if the underlying UHD version does not support
     *         the power API.
     */
    virtual ::uhd::meta_range_t get_power_range(size_t chan = 0) = 0;

    /*!
     * Set the antenna to use for a given channel.
     *
     * \param ant the antenna string
     * \param chan the channel index 0 to N-1
     */
    virtual void set_antenna(const std::string& ant, size_t chan = 0) = 0;

    /*!
     * Get the antenna in use.
     *
     * \param chan the channel index 0 to N-1
     * \return the antenna string
     */
    virtual std::string get_antenna(size_t chan = 0) = 0;

    /*!
     * Get a list of possible antennas on a given channel.
     *
     * \param chan the channel index 0 to N-1
     * \return a vector of antenna strings
     */
    virtual std::vector<std::string> get_antennas(size_t chan = 0) = 0;

    /*!
     * Set the bandpass filter on the RF frontend.
     *
     * \param bandwidth the filter bandwidth in Hz
     * \param chan the channel index 0 to N-1
     */
    virtual void set_bandwidth(double bandwidth, size_t chan = 0) = 0;

    /*!
     * Get the bandpass filter setting on the RF frontend.
     *
     * \param chan the channel index 0 to N-1
     * \return bandwidth of the filter in Hz
     */
    virtual double get_bandwidth(size_t chan = 0) = 0;

    /*!
     * Get the bandpass filter range of the RF frontend.
     *
     * \param chan the channel index 0 to N-1
     * \return the range of the filter bandwidth in Hz
     */
    virtual ::uhd::freq_range_t get_bandwidth_range(size_t chan = 0) = 0;

    /*!
     * Get an RF frontend sensor value.
     * \param name the name of the sensor
     * \param chan the channel index 0 to N-1
     * \return a sensor value object
     */
    virtual ::uhd::sensor_value_t get_sensor(const std::string& name,
                                             size_t chan = 0) = 0;

    /*!
     * Get a list of possible RF frontend sensor names.
     * \param chan the channel index 0 to N-1
     * \return a vector of sensor names
     */
    virtual std::vector<std::string> get_sensor_names(size_t chan = 0) = 0;

    //! DEPRECATED use get_sensor
    ::uhd::sensor_value_t get_dboard_sensor(const std::string& name, size_t chan = 0)
    {
        return this->get_sensor(name, chan);
    }

    //! DEPRECATED use get_sensor_names
    std::vector<std::string> get_dboard_sensor_names(size_t chan = 0)
    {
        return this->get_sensor_names(chan);
    }

    /*!
     * Get a motherboard sensor value.
     *
     * \param name the name of the sensor
     * \param mboard the motherboard index 0 to M-1
     * \return a sensor value object
     */
    virtual ::uhd::sensor_value_t get_mboard_sensor(const std::string& name,
                                                    size_t mboard = 0) = 0;

    /*!
     * Get a list of possible motherboard sensor names.
     *
     * \param mboard the motherboard index 0 to M-1
     * \return a vector of sensor names
     */
    virtual std::vector<std::string> get_mboard_sensor_names(size_t mboard = 0) = 0;

    /*!
     * Get the currently set time source.
     *
     * \param mboard which motherboard to get the config
     * \return the string representing the time source
     */
    virtual std::string get_time_source(const size_t mboard) = 0;

    /*!
     * Get a list of possible time sources.
     *
     * \param mboard which motherboard to get the list
     * \return a vector of strings for possible settings
     */
    virtual std::vector<std::string> get_time_sources(const size_t mboard) = 0;

    /*!
     * Set the clock source for the usrp device.
     *
     * This sets the source for a 10 MHz reference clock.
     * Typical options for source: internal, external, MIMO.
     *
     * \param source a string representing the clock source
     * \param mboard which motherboard to set the config
     */
    virtual void set_clock_source(const std::string& source, const size_t mboard = 0) = 0;

    /*!
     * Get the currently set clock source.
     *
     * \param mboard which motherboard to get the config
     * \return the string representing the clock source
     */
    virtual std::string get_clock_source(const size_t mboard) = 0;

    /*!
     * Get a list of possible clock sources.
     *
     * \param mboard which motherboard to get the list
     * \return a vector of strings for possible settings
     */
    virtual std::vector<std::string> get_clock_sources(const size_t mboard) = 0;

    /*!
     * Get the master clock rate.
     *
     * \param mboard the motherboard index 0 to M-1
     * \return the clock rate in Hz
     */
    virtual double get_clock_rate(size_t mboard = 0) = 0;

    /*!
     * Set the master clock rate.
     *
     * \param rate the new rate in Hz
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_clock_rate(double rate, size_t mboard = 0) = 0;

    /*!
     * Get the current time registers.
     *
     * \param mboard the motherboard index 0 to M-1
     * \return the current usrp time
     */
    virtual ::uhd::time_spec_t get_time_now(size_t mboard = 0) = 0;

    /*!
     * Get the time when the last pps pulse occurred.
     * \param mboard the motherboard index 0 to M-1
     * \return the current usrp time
     */
    virtual ::uhd::time_spec_t get_time_last_pps(size_t mboard = 0) = 0;

    /*!
     * Sets the time registers immediately.
     * \param time_spec the new time
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_time_now(const ::uhd::time_spec_t& time_spec, size_t mboard = 0) = 0;

    /*!
     * Set the time registers at the next pps.
     * \param time_spec the new time
     */
    virtual void set_time_next_pps(const ::uhd::time_spec_t& time_spec) = 0;

    /*!
     * Sync the time registers with an unknown pps edge.
     * \param time_spec the new time
     */
    virtual void set_time_unknown_pps(const ::uhd::time_spec_t& time_spec) = 0;

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
    virtual void set_command_time(const ::uhd::time_spec_t& time_spec,
                                  size_t mboard = 0) = 0;

    /*!
     * Clear the command time so future commands are sent ASAP.
     *
     * \param mboard which motherboard to set the config
     */
    virtual void clear_command_time(size_t mboard = 0) = 0;

    /*!
     * Get access to the underlying uhd dboard iface object.
     *
     * \return the dboard_iface object
     */
    virtual ::uhd::usrp::dboard_iface::sptr get_dboard_iface(size_t chan = 0) = 0;

    /*!
     * Get access to the underlying uhd device object.
     *
     * NOTE: This function is only available in C++.
     * \return the multi usrp device object
     */
    virtual ::uhd::usrp::multi_usrp::sptr get_device(void) = 0;

    /*!
     * Perform write on the user configuration register bus. These
     * only exist if the user has implemented custom setting
     * registers in the device FPGA.
     *
     * \param addr 8-bit register address
     * \param data 32-bit register value
     * \param mboard which motherboard to set the user register
     */
    virtual void
    set_user_register(const uint8_t addr, const uint32_t data, size_t mboard = 0) = 0;

    /*!
     * Set the time source for the USRP device.
     *
     * This sets the method of time synchronization,
     * typically a pulse per second or an encoded time.
     * Typical options for source: external, MIMO.
     * \param source a string representing the time source
     * \param mboard which motherboard to set the config
     */
    virtual void set_time_source(const std::string& source, const size_t mboard = 0) = 0;

    /*!
     * Update the stream args for this device.
     *
     * This update will only take effect after a restart of the
     * streaming, or before streaming and after construction.
     * This will also delete the current streamer.
     * Note you cannot change the I/O signature of this block using
     * this function, or it will throw.
     *
     * It is possible to leave the 'channels' fields of \p stream_args
     * unset. In this case, the previous channels field is used.
     *
     * \param stream_args New stream args.
     * \throws std::runtime_error if new settings are invalid.
     */
    virtual void set_stream_args(const ::uhd::stream_args_t& stream_args) = 0;

    /*******************************************************************
     * GPIO methods
     ******************************************************************/
    /*!
     * Enumerate GPIO banks on the current device.
     * \param mboard the motherboard index 0 to M-1
     * \return a list of string for each bank name
     */
    virtual std::vector<std::string> get_gpio_banks(const size_t mboard) = 0;

    /*!
     * Set a GPIO attribute on a particular GPIO bank.
     * Possible attribute names:
     *  - CTRL - 1 for ATR mode 0 for GPIO mode
     *  - DDR - 1 for output 0 for input
     *  - OUT - GPIO output level (not ATR mode)
     *  - ATR_0X - ATR idle state
     *  - ATR_RX - ATR receive only state
     *  - ATR_TX - ATR transmit only state
     *  - ATR_XX - ATR full duplex state
     * \param bank the name of a GPIO bank
     * \param attr the name of a GPIO attribute
     * \param value the new value for this GPIO bank
     * \param mask the bit mask to effect which pins are changed
     * \param mboard the motherboard index 0 to M-1
     */
    virtual void set_gpio_attr(const std::string& bank,
                               const std::string& attr,
                               const uint32_t value,
                               const uint32_t mask = 0xffffffff,
                               const size_t mboard = 0) = 0;

    /*!
     * Get a GPIO attribute on a particular GPIO bank.
     * Possible attribute names:
     *  - CTRL - 1 for ATR mode 0 for GPIO mode
     *  - DDR - 1 for output 0 for input
     *  - OUT - GPIO output level (not ATR mode)
     *  - ATR_0X - ATR idle state
     *  - ATR_RX - ATR receive only state
     *  - ATR_TX - ATR transmit only state
     *  - ATR_XX - ATR full duplex state
     *  - READBACK - readback input GPIOs
     * \param bank the name of a GPIO bank
     * \param attr the name of a GPIO attribute
     * \param mboard the motherboard index 0 to M-1
     * \return the value set for this attribute
     */
    virtual uint32_t get_gpio_attr(const std::string& bank,
                                   const std::string& attr,
                                   const size_t mboard = 0) = 0;

    /*! Enumerate the available filters in the signal path.
     *
     * \param chan Channel index
     *
     * \return a vector of strings representing the selected filter names.
     */
    virtual std::vector<std::string> get_filter_names(const size_t chan = 0) = 0;

    /*! Write back a filter obtained by get_filter() to the signal path.
     *
     * This filter can be a modified version of the originally returned one.
     * The information about Rx or Tx is contained in the path parameter.
     * \param path the name of the filter as returned from get_filter_names().
     * \param filter the filter_info_base::sptr of the filter object to be written
     * \param chan Channel index
     */
    virtual void set_filter(const std::string& path,
                            ::uhd::filter_info_base::sptr filter,
                            const size_t chan = 0) = 0;

    /*! Return the filter object for the given name.
     *
     * \param path the name of the filter as returned from get_filter_names()
     * \param chan Channel index
     * \return the filter object
     */
    virtual ::uhd::filter_info_base::sptr get_filter(const std::string& path,
                                                     const size_t chan = 0) = 0;

    /*!
     * Returns identifying information about this USRP's configuration.
     * Returns motherboard ID, name, and serial.
     * Returns daughterboard TX ID, subdev name and spec, serial, and antenna.
     * \param chan channel index 0 to N-1
     * \return TX info
     */
    virtual ::uhd::dict<std::string, std::string> get_usrp_info(size_t chan = 0) = 0;
};

} /* namespace uhd */
} /* namespace gr */

#endif /* INCLUDED_GR_UHD_USRP_BLOCK_H */
