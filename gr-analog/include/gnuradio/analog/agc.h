/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC_H
#define INCLUDED_ANALOG_AGC_H

#include <gnuradio/analog/api.h>
#include <gnuradio/gr_complex.h>
#include <cmath>

namespace gr {
namespace analog {
namespace kernel {

/*!
 * \brief high performance Automatic Gain Control class for complex signals.
 * \ingroup level_controllers_blk
 *
 * \details
 * For Power the absolute value of the complex number is used.
 */
class ANALOG_API agc_cc
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
    agc_cc(float rate = 1e-4,
           float reference = 1.0,
           float gain = 1.0,
           float max_gain = 0.0)
        : _rate(rate), _reference(reference), _gain(gain), _max_gain(max_gain){};

    virtual ~agc_cc(){};

    float rate() const { return _rate; }
    float reference() const { return _reference; }
    float gain() const { return _gain; }
    float max_gain() const { return _max_gain; }

    void set_rate(float rate) { _rate = rate; }
    void set_reference(float reference) { _reference = reference; }
    void set_gain(float gain) { _gain = gain; }
    void set_max_gain(float max_gain) { _max_gain = max_gain; }

    gr_complex scale(gr_complex input)
    {
        gr_complex output = input * _gain;

        _gain += _rate * (_reference - std::sqrt(output.real() * output.real() +
                                                 output.imag() * output.imag()));
        if (_max_gain > 0.0 && _gain > _max_gain) {
            _gain = _max_gain;
        }
        return output;
    }

    void scaleN(gr_complex output[], const gr_complex input[], unsigned n)
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

/*!
 * \brief high performance Automatic Gain Control class for float signals.
 *
 * Power is approximated by absolute value
 */
class ANALOG_API agc_ff
{
public:
    /*!
     * Construct a floating point value AGC loop implementation object.
     *
     * \param rate the update rate of the loop.
     * \param reference reference value to adjust signal power to.
     * \param gain initial gain value.
     * \param max_gain maximum gain value (0 for unlimited).
     */
    agc_ff(float rate = 1e-4,
           float reference = 1.0,
           float gain = 1.0,
           float max_gain = 0.0)
        : _rate(rate), _reference(reference), _gain(gain), _max_gain(max_gain){};

    ~agc_ff(){};

    float rate() const { return _rate; }
    float reference() const { return _reference; }
    float gain() const { return _gain; }
    float max_gain() const { return _max_gain; }

    void set_rate(float rate) { _rate = rate; }
    void set_reference(float reference) { _reference = reference; }
    void set_gain(float gain) { _gain = gain; }
    void set_max_gain(float max_gain) { _max_gain = max_gain; }

    float scale(float input)
    {
        float output = input * _gain;
        _gain += (_reference - fabsf(output)) * _rate;
        if (_max_gain > 0.0 && _gain > _max_gain)
            _gain = _max_gain;
        return output;
    }

    void scaleN(float output[], const float input[], unsigned n)
    {
        for (unsigned i = 0; i < n; i++)
            output[i] = scale(input[i]);
    }

protected:
    float _rate;      // adjustment rate
    float _reference; // reference value
    float _gain;      // current gain
    float _max_gain;  // maximum gain
};

} /* namespace kernel */
} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC_H */
