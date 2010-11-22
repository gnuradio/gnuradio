/* -*- c++ -*- */
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

#ifndef INCLUDED_UHD_MIMO_SINK_H
#define INCLUDED_UHD_MIMO_SINK_H

#include <gr_sync_block.h>
#include <uhd/usrp/mimo_usrp.hpp>

class uhd_mimo_sink;

boost::shared_ptr<uhd_mimo_sink> uhd_make_mimo_sink(
    size_t num_channels,
    const std::string &args,
    const uhd::io_type_t::tid_t &type
);

/***********************************************************************
 * DEPRECATED
 **********************************************************************/
class /*UHD_DEPRECATED*/ uhd_mimo_sink : public gr_sync_block{
public:

    /*!
     * Set the IO signature for this block.
     * \param sig the input signature
     */
    uhd_mimo_sink(gr_io_signature_sptr sig);

    /*!
     * Set the subdevice specification.
     * \param chan the channel number from 0 to N-1
     * \param spec the subdev spec markup string
     */
    virtual void set_subdev_spec(size_t chan, const std::string &spec) = 0;

    /*!
     * Set the sample rate for the usrp device (across all mboards).
     * \param rate a new rate in Sps
     */
    virtual void set_samp_rate_all(double rate) = 0;

    /*!
     * Get the sample rate for the usrp device (across all mboards).
     * This is the actual sample rate and may differ from the rate set.
     * \return the actual rate in Sps
     */
    virtual double get_samp_rate_all(void) = 0;

    /*!
     * Tune the usrp device to the desired center frequency.
     * \param chan the channel number from 0 to N-1
     * \param freq the desired frequency in Hz
     * \return a tune result with the actual frequencies
     */
    virtual uhd::tune_result_t set_center_freq(size_t chan, double freq) = 0;

    /*!
     * Get the tunable frequency range.
     * \param chan the channel number from 0 to N-1
     * \return the frequency range in Hz
     */
    virtual uhd::freq_range_t get_freq_range(size_t chan) = 0;

    /*!
     * Set the gain for the dboard.
     * \param chan the channel number from 0 to N-1
     * \param gain the gain in dB
     */
    virtual void set_gain(size_t chan, float gain) = 0;

    /*!
     * Get the actual dboard gain setting.
     * \param chan the channel number from 0 to N-1
     * \return the actual gain in dB
     */
    virtual float get_gain(size_t chan) = 0;

    /*!
     * Get the settable gain range.
     * \param chan the channel number from 0 to N-1
     * \return the gain range in dB
     */
    virtual uhd::gain_range_t get_gain_range(size_t chan) = 0;

    /*!
     * Set the antenna to use.
     * \param chan the channel number from 0 to N-1
     * \param ant the antenna string
     */
    virtual void set_antenna(size_t chan, const std::string &ant) = 0;

    /*!
     * Get the antenna in use.
     * \param chan the channel number from 0 to N-1
     * \return the antenna string
     */
    virtual std::string get_antenna(size_t chan) = 0;

    /*!
     * Get a list of possible antennas.
     * \param chan the channel number from 0 to N-1
     * \return a vector of antenna strings
     */
    virtual std::vector<std::string> get_antennas(size_t chan) = 0;

    /*!
     * Get the current time registers.
     * \return the current usrp time
     */
    virtual uhd::time_spec_t get_time_now(void) = 0;

    /*!
     * Set the time registers at the next pps (across all mboards).
     * \param time_spec the new time
     */
    virtual void set_time_next_pps(const uhd::time_spec_t &time_spec) = 0;

    /*!
     * Get access to the underlying uhd device object.
     * \return the mimo usrp device object
     */
    virtual uhd::usrp::mimo_usrp::sptr get_device(void) = 0;
};

#endif /* INCLUDED_UHD_MIMO_SINK_H */
