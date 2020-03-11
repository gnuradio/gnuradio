/* -*- c++ -*- */
/*
 * Copyright 2008, 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_FMDET_CF_IMPL_H
#define INCLUDED_ANALOG_FMDET_CF_IMPL_H

#include <gnuradio/analog/fmdet_cf.h>
//#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

class fmdet_cf_impl : public fmdet_cf
{
private:
    gr_complex d_S1, d_S2, d_S3, d_S4;
    float d_freq, d_freqlo, d_freqhi, d_scl, d_bias;
    // kernel::fir_filter_ccf* d_filter;

public:
    fmdet_cf_impl(float samplerate, float freq_low, float freq_high, float scl);
    ~fmdet_cf_impl();

    void set_scale(float scl);
    void set_freq_range(float freq_low, float freq_high);

    float freq() const { return d_freq; }
    float freq_high() const { return d_freqhi; }
    float freq_low() const { return d_freqlo; }
    float scale() const { return d_scl; }
    float bias() const { return d_bias; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_FMDET_CF_IMPL_H */
