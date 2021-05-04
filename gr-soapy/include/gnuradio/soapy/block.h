/* -*- c++ -*- */
/*
 * Copyright 2021 Jeff Long
 * Copyright 2018-2021 Libre Space Foundation <http://libre.space/>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_BLOCK_H
#define INCLUDED_GR_SOAPY_BLOCK_H

#include <gnuradio/block.h>
#include <gnuradio/soapy/api.h>
#include <cstdint>
#include <string>
#include <vector>

namespace gr {
namespace soapy {

class SOAPY_API block : virtual public gr::block
{
public:
    /*!
     * Set the frontend mapping of available DSP units to RF frontends.
     * This mapping controls channel mapping and channel availability.
     * \param frontend_mapping a vendor-specific mapping string
     */
    virtual void set_frontend_mapping(const std::string& frontend_mapping) = 0;

    /*!
     * Set sample rate
     * \param channel an available channel
     * \param sample_rate samples per second
     */
    virtual void set_sample_rate(size_t channel, double sample_rate) = 0;

    /*!
     * Get the baseband sample rate of the RX chain.
     * \param channel an available channel on the device
     * \return the sample rate in samples per second
     */
    virtual double get_sample_rate(size_t channel) const = 0;

    /*!
     * Set device center frequency
     * \param channel an available channel
     * \param freq frequency in Hz
     */
    virtual void set_frequency(size_t channel, double freq) = 0;

    /*!
     * Set center frequency of a tunable element
     * \param channel an available channel
     * \param name an available element name
     * \param freq frequency in Hz
     */
    virtual void set_frequency(size_t channel, const std::string& name, double freq) = 0;

    /*!
     * Get the down conversion frequency of the chain.
     * \param channel an available channel on the device
     * \return the center frequency in Hz
     */
    virtual double get_frequency(size_t channel) const = 0;

    /*!
     * Set filter bandwidth
     * \param channel an available channel
     * \param bandwidth filter width in Hz
     */
    virtual void set_bandwidth(size_t channel, double bandwidth) = 0;

    /*!
     * Get baseband filter width of the RX chain.
     * \param channel an available channel on the device
     * \return the baseband filter width in Hz
     */
    virtual double get_bandwidth(size_t channel) const = 0;

    /*!
     * List available antennas for a channel
     * @param channel channel index
     * @return available antenna names
     */
    virtual std::vector<std::string> list_antennas(int channel) const = 0;

    /*!
     * Set antenna for channel
     * \param channel an available channel
     * \param name an available antenna string name
     */
    virtual void set_antenna(size_t channel, const std::string& name) = 0;

    /*!
     * Get the selected antenna on RX chain.
     * \param channel an available channel on the device
     * \return the name of the selected antenna
     */
    virtual std::string get_antenna(size_t channel) const = 0;

    /*!
     * Return whether automatic gain control (AGC) is supported
     * \param channel an available channel
     */
    virtual bool has_gain_mode(size_t channel) const = 0;

    /*!
     * Set automatic gain control (AGC)
     * \param channel an available channel
     * \param enable true to enable AGC
     */
    virtual void set_gain_mode(size_t channel, bool enable) = 0;

    /*!
     * Get the automatic gain mode on the RX chain.
     * \param channel an available channel on the device
     * \return true for automatic gain setting
     */
    virtual bool get_gain_mode(size_t channel) const = 0;

    /*!
     * Set overall gain
     * The gain will be distributed automatically across available
     * elements according to Soapy API.
     * \param channel an available channel
     * \param gain overall gain value
     */
    virtual void set_gain(size_t channel, double gain) = 0;

    /*!
     * Set specific gain value
     * \param channel an available channel
     * \param name gain name to set
     * \param gain gain value
     */
    virtual void set_gain(size_t channel, const std::string& name, double gain) = 0;

    /*!
     * Get the overall value of the gain elements in a chain
     * \param channel an available channel on the device
     * \return the value of the gain in dB
     */
    virtual double get_gain(size_t channel) const = 0;

    /*!
     * Return whether frequency correction is supported
     * \param channel an available channel
     */
    virtual bool has_frequency_correction(size_t channel) const = 0;

    /*!
     * Set frequency correction
     * \param channel an available channel
     * \param freq_correction in PPM
     */
    virtual void set_frequency_correction(size_t channel, double freq_correction) = 0;

    /*!
     * Get the frequency correction value.
     * \param channel an available channel on the device
     * \return the correction value in PPM
     */
    virtual double get_frequency_correction(size_t channel) const = 0;

    /*!
     * Return whether DC offset mode can be set
     * \param channel an available channel
     */
    virtual bool has_dc_offset_mode(size_t channel) const = 0;

    /*!
     * Set DC offset mode
     * \param channel an available channel
     * \param automatic true to set automatic DC removal
     */
    virtual void set_dc_offset_mode(size_t channel, bool automatic) = 0;

    /*!
     * Get the automatic DC offset correction mode.
     * \param channel an available channel on the device
     * \return true for automatic offset correction
     */
    virtual bool get_dc_offset_mode(size_t channel) const = 0;

    /*!
     * Return whether manual dc offset correction is supported
     * \param channel an available channel
     */
    virtual bool has_dc_offset(size_t channel) const = 0;

    /*!
     * Set dc offset correction
     * \param channel an available channel
     * \param dc_offset complex dc offset correction
     */
    virtual void set_dc_offset(size_t channel, const gr_complexd& dc_offset) = 0;

    /*!
     * Get the DC offset correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual gr_complexd get_dc_offset(size_t channel) const = 0;

    /*!
     * Return whether manual IQ balance correction is supported
     * \param channel an available channel
     */
    virtual bool has_iq_balance(size_t channel) const = 0;

    /*!
     * Set IQ balance correction
     * \param channel an available channel
     * \param iq_balance complex iq balance correction
     */
    virtual void set_iq_balance(size_t channel, const gr_complexd& iq_balance) = 0;

    /*!
     * Get the IQ balance correction.
     * \param channel an available channel on the device
     * \return the relative correction (1.0 max)
     */
    virtual gr_complexd get_iq_balance(size_t channel) const = 0;

    /*!
     * Set master clock rate
     * \param clock_rate clock rate in Hz
     */
    virtual void set_master_clock_rate(double clock_rate) = 0;

    /*!
     * Get the master clock rate of the device.
     * \return the clock rate in Hz
     */
    virtual double get_master_clock_rate() const = 0;

    /*!
     * Set the clock source
     * \param clock_source an available clock source
     */
    virtual void set_clock_source(const std::string& clock_source) = 0;

    /*!
     * Get the clock source of the device
     * \return the name of the clock source
     */
    virtual std::string get_clock_source() const = 0;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_BLOCK_H */
