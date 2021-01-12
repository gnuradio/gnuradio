/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace blocks {

/*!
 * \brief This block to removes that center frequency IQ DC spike with a slight variation.
 * \p It automatically calculates the offset then switches to straight DC offset mode to
 * \p prevent any possible IIR filtering after it's been tuned.  However, if frequency
 * \p or upstream gain is changed, it must retune, so frequency and upstream gain are
 * \p all taken as parameters and monitored for changes.
 * \ingroup iq_correction
 *
 */
class BLOCKS_API correctiq_auto : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<correctiq_auto> sptr;

    /*!
     * \brief Return a shared_ptr to a new instance of correctiq::correctiq_auto.
     *
     * To avoid accidental use of raw pointers, correctiq::correctiq_auto's
     * constructor is in a private implementation
     * class. correctiq::correctiq_auto::make is the public interface for
     * creating new instances.
     */

    virtual double get_freq() const = 0;
    virtual float get_gain() const = 0;

    virtual void set_freq(double newValue) = 0;
    virtual void set_gain(float newValue) = 0;

    static sptr make(double samp_rate, double freq, float gain, float sync_window);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_H */
