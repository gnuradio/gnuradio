/* -*- c++ -*- */
/*
 * Copyright 2009,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_CHANNEL_MODEL_H
#define INCLUDED_CHANNELS_CHANNEL_MODEL_H

#include <gnuradio/channels/api.h>
#include <gnuradio/hier_block2.h>
#include <gnuradio/types.h>

namespace gr {
namespace channels {

/*!
 * \brief Basic channel simulator.
 * \ingroup channel_models_blk
 *
 * \details
 * This block implements a basic channel model simulator that can
 * be used to help evaluate, design, and test various signals,
 * waveforms, and algorithms.
 *
 * This model allows the user to set the voltage of an AWGN noise
 * source (\p noise_voltage), a (normalized) frequency offset (\p
 * frequency_offset), a sample timing offset (\p epsilon), and a
 * seed (\p noise_seed) to randomize or make reproducible the AWGN
 * noise source.
 *
 * Multipath can be approximated in this model by using a FIR
 * filter representation of a multipath delay profile with the
 * parameter \p taps.
 *
 * To simulate a channel with time-variant channel, use
 * gr::channels::channel_model2.
 */
class CHANNELS_API channel_model : virtual public hier_block2
{
public:
    // gr::channels::channel_model::sptr
    typedef std::shared_ptr<channel_model> sptr;

    /*! \brief Build the channel simulator.
     *
     * \param noise_voltage The AWGN noise level as a voltage (to be
     *                      calculated externally to meet, say, a
     *                      desired SNR).
     * \param frequency_offset The normalized frequency offset. 0 is
     *                         no offset; 0.25 would be, for a digital
     *                         modem, one quarter of the symbol rate.
     * \param epsilon The sample timing offset to emulate the
     *                different rates between the sample clocks of
     *                the transmitter and receiver. 1.0 is no difference.
     * \param taps Taps of a FIR filter to emulate a multipath delay profile.
     * \param noise_seed A random number generator seed for the noise source.
     * \param block_tags If true, tags will not be able to propagate through this block.
     */
    static sptr make(double noise_voltage = 0.0,
                     double frequency_offset = 0.0,
                     double epsilon = 1.0,
                     const std::vector<gr_complex>& taps = std::vector<gr_complex>(1, 1),
                     double noise_seed = 0,
                     bool block_tags = false);

    virtual void set_noise_voltage(double noise_voltage) = 0;
    virtual void set_frequency_offset(double frequency_offset) = 0;
    virtual void set_taps(const std::vector<gr_complex>& taps) = 0;
    virtual void set_timing_offset(double epsilon) = 0;

    virtual double noise_voltage() const = 0;
    virtual double frequency_offset() const = 0;
    virtual std::vector<gr_complex> taps() const = 0;
    virtual double timing_offset() const = 0;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_CHANNEL_MODEL_H */
