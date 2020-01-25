/* -*- c++ -*- */
/*
 * Copyright 2010,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H
#define INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H

#include <gnuradio/analog/frequency_modulator_fc.h>
#include <gnuradio/blocks/char_to_float.h>
#include <gnuradio/digital/cpmmod_bc.h>
#include <gnuradio/filter/interp_fir_filter.h>

namespace gr {
namespace digital {

class cpmmod_bc_impl : public cpmmod_bc
{
private:
    const int d_type;
    float d_index;
    int d_sps;
    int d_length;
    double d_beta;

protected:
    std::vector<float> d_taps;
    gr::blocks::char_to_float::sptr d_char_to_float;
    gr::filter::interp_fir_filter_fff::sptr d_pulse_shaper;
    analog::frequency_modulator_fc::sptr d_fm;

public:
    cpmmod_bc_impl(const std::string& name,
                   analog::cpm::cpm_type type,
                   float h,
                   int samples_per_sym,
                   int L,
                   double beta = 0.3);
    ~cpmmod_bc_impl();

    std::vector<float> taps() const;
    int type() const;
    float index() const;
    int samples_per_sym() const;
    int length() const;
    double beta() const;
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CPMMOD_BC_IMPL_H */
