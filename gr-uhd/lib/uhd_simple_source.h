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

#ifndef INCLUDED_UHD_SIMPLE_SOURCE_H
#define INCLUDED_UHD_SIMPLE_SOURCE_H

#include <gr_sync_block.h>
#include <uhd/usrp/simple_usrp.hpp>

class uhd_simple_source;

boost::shared_ptr<uhd_simple_source>
uhd_make_simple_source(const std::string &args, const uhd::io_type_t::tid_t &type);

class uhd_simple_source : public gr_sync_block{
public:
    uhd_simple_source(const std::string &args, const uhd::io_type_t &type);
    ~uhd_simple_source(void);

    /*!
     * Set the sample rate for the usrp device.
     * \param rate a new rate in Sps
     */
    void set_samp_rate(double rate);

    /*!
     * Get the sample rate for the usrp device.
     * This is the actual sample rate and may differ from the rate set.
     * \return the actual rate in Sps
     */
    double get_samp_rate(void);

    /*!
     * Tune the usrp device to the desired center frequency.
     * \param freq the desired frequency in Hz
     * \return a tune result with the actual frequencies
     */
    uhd::tune_result_t set_center_freq(double freq);

    /*!
     * Get the tunable frequency range.
     * \return the frequency range in Hz
     */
    uhd::freq_range_t get_freq_range(void);

    /*!
     * Set the gain for the dboard.
     * \param gain the gain in dB
     */
    void set_gain(float gain);

    /*!
     * Get the actual dboard gain setting.
     * \return the actual gain in dB
     */
    float get_gain(void);

    /*!
     * Get the settable gain range.
     * \return the gain range in dB
     */
    uhd::gain_range_t get_gain_range(void);

    /*!
     * Set the antenna to use.
     * \param ant the antenna string
     */
    void set_antenna(const std::string &ant);

    /*!
     * Get the antenna in use.
     * \return the antenna string
     */
    std::string get_antenna(void);

    /*!
     * Get a list of possible antennas.
     * \return a vector of antenna strings
     */
    std::vector<std::string> get_antennas(void);

    int work(
        int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items
    );

protected:
    uhd::usrp::simple_usrp::sptr _dev;
    const uhd::io_type_t _type;
    bool _is_streaming;
    void set_streaming(bool enb);
};

#endif /* INCLUDED_UHD_SIMPLE_SOURCE_H */
