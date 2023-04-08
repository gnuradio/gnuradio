/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_AGC3_IMPL_CC_H
#define INCLUDED_ANALOG_AGC3_IMPL_CC_H

#include <gnuradio/analog/agc3_cc.h>

namespace gr {
namespace analog {

class agc3_cc_impl : public agc3_cc
{
public:
    agc3_cc_impl(float attack_rate = 1e-1,
                 float decay_rate = 1e-2,
                 float reference = 1.0,
                 float gain = 1.0,
                 int iir_update_decim = 1,
                 float max_gain = 0.0);
    ~agc3_cc_impl() override;

    float attack_rate() const override;
    float decay_rate() const override;
    float reference() const override;
    float gain() const override;
    float max_gain() const override;

    void set_attack_rate(float rate) override;
    void set_decay_rate(float rate) override;
    void set_reference(float reference) override;
    void set_gain(float gain) override;
    void set_max_gain(float max_gain) override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

private:
    void test_and_log_value_domain(float value, std::string_view description);
    mutable gr::thread::mutex d_setter_mutex;
    float d_attack;
    float d_decay;
    float d_reference;
    float d_gain;
    float d_max_gain;
    unsigned int d_iir_update_decim;
    bool d_reset;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_AGC3_CC_IMPL_H */
