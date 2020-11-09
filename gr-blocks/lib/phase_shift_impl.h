/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PHASE_SHIFT_IMPL_H
#define INCLUDED_PHASE_SHIFT_IMPL_H

#include <gnuradio/blocks/phase_shift.h>

namespace gr {
namespace blocks {

class phase_shift_impl : public phase_shift
{
private:
    bool d_is_radians;
    float d_shift;
    gr_complex d_shift_cc;

public:
    phase_shift_impl(float shift, bool is_radians);
    ~phase_shift_impl() override;

    float get_shift() const override { return d_shift; }
    void set_shift(float new_value) override;

    void handle_msg_in(pmt::pmt_t msg);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_PHASE_SHIFT_IMPL_H */
