/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC2_H
#define INCLUDED_ANALOG_AGC2_H

#include <gnuradio/analog/api.h>
#include <gnuradio/gr_complex.h>
#include <cmath>

namespace gr {
namespace analog {
namespace kernel {

/*!
 * \brief high performance Automatic Gain Control class
 * \ingroup level_controllers_blk
 *
 * \details
 * For Power the absolute value of the complex number is used.
 */
class ANALOG_API agc2_cc
{
public:
    /*!
     * Construct a comple value AGC loop implementation object.
     *
     * \param attack_rate the update rate of the loop when in attack mode.
     * \param decay_rate the update rate of the loop when in decay mode.
     * \param reference reference value to adjust signal power to.
     * \param gain initial gain value.
     * \param max_gain maximum gain value (0 for unlimited).
     */
    agc2_cc(float attack_rate = 1e-1,
            float decay_rate = 1e-2,
            float reference = 1.0,
            float gain = 1.0,
            float max_gain = 0.0)
        : _attack_rate(attack_rate),
          _decay_rate(decay_rate),
          _reference(reference),
          _gain(gain),
          _max_gain(max_gain){};

    float decay_rate() const { return _decay_rate; }
    float attack_rate() const { return _attack_rate; }
    float reference() const { return _reference; }
    float gain() const { return _gain; }
    float max_gain() const { return _max_gain; }

    void set_decay_rate(float rate) { _decay_rate = rate; }
    void set_attack_rate(float rate) { _attack_rate = rate; }
    void set_reference(float reference) { _reference = reference; }
    void set_gain(float gain) { _gain = gain; }
    void set_max_gain(float max_gain) { _max_gain = max_gain; }

    gr_complex scale(gr_complex input)
    {
        gr_complex output = input * _gain;

        float tmp = -_reference +
                    sqrt(output.real() * output.real() + output.imag() * output.imag());
        float rate = _decay_rate;
        if ((tmp) > _gain) {
            rate = _attack_rate;
        }
        _gain -= tmp * rate;

        // Not sure about this; will blow up if _gain < 0 (happens
        // when rates are too high), but is this the solution?
        if (_gain < 0.0)
            _gain = 10e-5;

        if (_max_gain > 0.0 && _gain > _max_gain) {
            _gain = _max_gain;
        }
        return output;
    }

    void scaleN(gr_complex output[], const gr_complex input[], unsigned n)
    {
        for (unsigned i = 0; i < n; i++)
            output[i] = scale(input[i]);
    }

protected:
    float _attack_rate; // attack rate for fast changing signals
    float _decay_rate;  // decay rate for slow changing signals
    float _reference;   // reference value
    float _gain;        // current gain
    float _max_gain;    // max allowable gain
};


class ANALOG_API agc2_ff
{
public:
    /*!
     * Construct a floating point value AGC loop implementation object.
     *
     * \param attack_rate the update rate of the loop when in attack mode.
     * \param decay_rate the update rate of the loop when in decay mode.
     * \param reference reference value to adjust signal power to.
     * \param gain initial gain value.
     * \param max_gain maximum gain value (0 for unlimited).
     */
    agc2_ff(float attack_rate = 1e-1,
            float decay_rate = 1e-2,
            float reference = 1.0,
            float gain = 1.0,
            float max_gain = 0.0)
        : _attack_rate(attack_rate),
          _decay_rate(decay_rate),
          _reference(reference),
          _gain(gain),
          _max_gain(max_gain){};

    float attack_rate() const { return _attack_rate; }
    float decay_rate() const { return _decay_rate; }
    float reference() const { return _reference; }
    float gain() const { return _gain; }
    float max_gain() const { return _max_gain; }

    void set_attack_rate(float rate) { _attack_rate = rate; }
    void set_decay_rate(float rate) { _decay_rate = rate; }
    void set_reference(float reference) { _reference = reference; }
    void set_gain(float gain) { _gain = gain; }
    void set_max_gain(float max_gain) { _max_gain = max_gain; }

    float scale(float input)
    {
        float output = input * _gain;

        float tmp = (fabsf(output)) - _reference;
        float rate = _decay_rate;
        if (fabsf(tmp) > _gain) {
            rate = _attack_rate;
        }
        _gain -= tmp * rate;

        // Not sure about this
        if (_gain < 0.0)
            _gain = 10e-5;

        if (_max_gain > 0.0 && _gain > _max_gain) {
            _gain = _max_gain;
        }
        return output;
    }

    void scaleN(float output[], const float input[], unsigned n)
    {
        for (unsigned i = 0; i < n; i++)
            output[i] = scale(input[i]);
    }

protected:
    float _attack_rate; // attack_rate for fast changing signals
    float _decay_rate;  // decay rate for slow changing signals
    float _reference;   // reference value
    float _gain;        // current gain
    float _max_gain;    // maximum gain
};

} /* namespace kernel */
} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC2_H */
