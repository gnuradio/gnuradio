/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_MEAS_EVM_CC_IMPL_H
#define INCLUDED_DIGITAL_MEAS_EVM_CC_IMPL_H

#include <gnuradio/digital/meas_evm_cc.h>

namespace gr {
namespace digital {

class meas_evm_cc_impl : public meas_evm_cc
{
private:
    constellation_sptr d_cons;
    const std::vector<gr_complex> d_cons_points;
    float d_cons_mag;
    const evm_measurement_t d_meas_type;

public:
    meas_evm_cc_impl(constellation_sptr cons, evm_measurement_t meas_type);
    ~meas_evm_cc_impl() override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MEAS_EVM_CC_IMPL_H */
