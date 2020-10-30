/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_SQUELCH_BASE_CC_IMPL_H
#define INCLUDED_GR_SQUELCH_BASE_CC_IMPL_H

#include <gnuradio/analog/squelch_base_cc.h>

namespace gr {
namespace analog {

class squelch_base_cc_impl : public squelch_base_cc
{
private:
    int d_ramp;
    int d_ramped;
    bool d_gate;
    double d_envelope;
    enum { ST_MUTED, ST_ATTACK, ST_UNMUTED, ST_DECAY } d_state;
    const pmt::pmt_t d_sob_key, d_eob_key;
    bool d_tag_next_unmuted;

protected:
    void update_state(const gr_complex& sample) override{};
    bool mute() const override { return false; };

public:
    squelch_base_cc_impl(const char* name, int ramp, bool gate);
    ~squelch_base_cc_impl() override;

    int ramp() const override;
    void set_ramp(int ramp) override;
    bool gate() const override;
    void set_gate(bool gate) override;
    bool unmuted() const override;

    std::vector<float> squelch_range() const override = 0;

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_SQUELCH_BASE_IMPL_FF_H */
