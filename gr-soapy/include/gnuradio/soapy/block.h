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
     * Set sample rate
     * \param channel an available channel
     * \param sample_rate samples per second
     */
    virtual void set_sample_rate(size_t channel, double sample_rate) = 0;

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
     * Set filter bandwidth
     * \param channel an available channel
     * \param bandwidth filter width in Hz
     */
    virtual void set_bandwidth(size_t channel, double bandwidth) = 0;

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
     * Set overall gain
     * The gain will be distributed automatically across available
     * elements according to Soapy API.
     * \param channel an available channel
     * \param gain overall gain value
     */
    virtual void set_gain(size_t channel, float gain) = 0;

    /*!
     * Set specific gain value
     * \param channel an available channel
     * \param name gain name to set
     * \param gain gain value
     */
    virtual void set_gain(size_t channel, const std::string& name, float gain) = 0;

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
     * Return whether manual dc offset correction is supported
     * \param channel an available channel
     */
    virtual bool has_dc_offset(size_t channel) const = 0;

    /*!
     * Set dc offset correction
     * \param channel an available channel
     * \param dc_offset complex dc offset correction
     */
    virtual void set_dc_offset(size_t channel, gr_complexd dc_offset) = 0;

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
    virtual void set_iq_balance(size_t channel, gr_complexd iq_balance) = 0;

    /*!
     * Set master clock rate
     * \param clock_rate clock rate in Hz
     */
    virtual void set_master_clock_rate(double clock_rate) = 0;

    /*!
     * Set the clock source
     * \param clock_source an available clock source
     */
    virtual void set_clock_source(const std::string& clock_source) = 0;
};

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_BLOCK_H */
