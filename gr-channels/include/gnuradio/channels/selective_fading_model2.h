/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_H
#define INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_H

#include <gnuradio/channels/api.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/types.h>

namespace gr {
namespace channels {

constexpr float default_delays2[] = { 1.0, 1.9, 2.7 };
constexpr float default_mags2[] = { 1.0, 0.95, 0.8 };

/*!
 * \brief fading simulator
 * \ingroup channel_models_blk
 *
 * \details
 * This block implements a basic fading model simulator that can
 * be used to help evaluate, design, and test various signals,
 * waveforms, and algorithms.
 */
class CHANNELS_API selective_fading_model2 : virtual public sync_block
{
public:
    // gr::channels::channel_model2::sptr
    typedef boost::shared_ptr<selective_fading_model2> sptr;

    /*! \brief Build the channel simulator.
     *
     * \param N      the number of sinusoids to use in simulating the channel; 8 is a good
     * value \param fDTs   normalized maximum Doppler frequency, fD * Ts \param LOS
     * include Line-of-Site path? selects between Rayleigh (NLOS) and Rician (LOS) models
     * \param K      Rician factor (ratio of the specular power to the scattered power)
     * \param seed   a random number to seed the noise generators
     * \param delays a vector of values that specify the time delay of each impulse
     * \param delays_std a vector of values that specify the gaussian step size std of
     * delays \param delays_maxdev a vector of values that specify the max deviation of
     * each delay component \param mags   a vector of values that specify the magnitude of
     * each impulse \param ntaps  the number of filter taps
     */
    static sptr make(unsigned int N,
                     float fDTs,
                     bool LOS,
                     float K,
                     uint32_t seed,
                     std::vector<float> delays,
                     std::vector<float> delays_std,
                     std::vector<float> delays_maxdev,
                     std::vector<float> mags,
                     unsigned int ntaps);

    virtual float fDTs() = 0;
    virtual float K() = 0;
    virtual float step() = 0;

    virtual void set_fDTs(float fDTs) = 0;
    virtual void set_K(float K) = 0;
    virtual void set_step(float step) = 0;
};

} /* namespace channels */
} /* namespace gr */

#endif /* INCLUDED_CHANNELS_SELECTIVE_FADING_MODEL2_H */
