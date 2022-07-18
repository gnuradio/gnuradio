/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/gr_complex.h>
#include <gnuradio/kernel/api.h>
#include <cmath>

namespace gr {
namespace kernel {
namespace analog {

/*!
 * \brief high performance Automatic Gain Control class for complex signals.
 * \ingroup level_controllers_blk
 *
 * \details
 * For Power the absolute value of the complex number is used.
 */
template <class T>
class agc
{
public:
    /*!
     * Construct a complex value AGC loop implementation object.
     *
     * \param rate the update rate of the loop.
     * \param reference reference value to adjust signal power to.
     * \param gain initial gain value.
     * \param max_gain maximum gain value (0 for unlimited).
     */
    agc(float rate = 1e-4, float reference = 1.0, float gain = 1.0, float max_gain = 0.0)
        : _rate(rate), _reference(reference), _gain(gain), _max_gain(max_gain){};

    virtual ~agc(){};

    float rate() const { return _rate; }
    float reference() const { return _reference; }
    float gain() const { return _gain; }
    float max_gain() const { return _max_gain; }

    void set_rate(float rate) { _rate = rate; }
    void set_reference(float reference) { _reference = reference; }
    void set_gain(float gain) { _gain = gain; }
    void set_max_gain(float max_gain) { _max_gain = max_gain; }

    T scale(T input);

    void scaleN(T output[], const T input[], unsigned n)
    {
        for (unsigned i = 0; i < n; i++) {
            output[i] = scale(input[i]);
        }
    }

protected:
    float _rate;      // adjustment rate
    float _reference; // reference value
    float _gain;      // current gain
    float _max_gain;  // max allowable gain
};


} // namespace analog
} // namespace kernel
} /* namespace gr */
