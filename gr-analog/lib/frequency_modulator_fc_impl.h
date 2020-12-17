/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_IMPL_H
#define INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_IMPL_H

#include <gnuradio/analog/frequency_modulator_fc.h>

namespace gr {
namespace analog {

class frequency_modulator_fc_impl : public frequency_modulator_fc
{
private:
    float d_sensitivity;
    float d_phase;

public:
    frequency_modulator_fc_impl(float sensitivity);
    ~frequency_modulator_fc_impl() override;

    void set_sensitivity(float sens) override { d_sensitivity = sens; }
    float sensitivity() const override { return d_sensitivity; }

    void setup_rpc() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FREQUENCY_MODULATOR_FC_IMPL_H */
