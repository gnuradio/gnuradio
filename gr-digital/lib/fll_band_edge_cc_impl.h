/* -*- c++ -*- */
/*
 * Copyright 2009,2011,2012,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_FLL_BAND_EDGE_CC_IMPL_H
#define INCLUDED_DIGITAL_FLL_BAND_EDGE_CC_IMPL_H

#include <gnuradio/blocks/control_loop.h>
#include <gnuradio/digital/fll_band_edge_cc.h>
#include <gnuradio/filter/fir_filter_with_buffer.h>

namespace gr {
namespace digital {

class fll_band_edge_cc_impl : public fll_band_edge_cc
{
private:
    float d_sps;
    float d_rolloff;
    int d_filter_size;

    std::vector<gr_complex> d_taps_lower;
    std::vector<gr_complex> d_taps_upper;
    bool d_updated;
    std::unique_ptr<gr::filter::kernel::fir_filter_with_buffer_ccc> d_filter_lower;
    std::unique_ptr<gr::filter::kernel::fir_filter_with_buffer_ccc> d_filter_upper;

    /*!
     * Design the band-edge filter based on the number of samples
     * per symbol, filter rolloff factor, and the filter size
     *
     * \param samps_per_sym    (float) Number of samples per symbol of signal
     * \param rolloff          (float) Rolloff factor of signal
     * \param filter_size      (int)   Size (in taps) of the filter
     */
    void design_filter(float samps_per_sym, float rolloff, int filter_size);

public:
    fll_band_edge_cc_impl(float samps_per_sym,
                          float rolloff,
                          int filter_size,
                          float bandwidth);

    void set_samples_per_symbol(float sps);
    void set_rolloff(float rolloff);
    void set_filter_size(int filter_size);

    float samples_per_symbol() const;
    float rolloff() const;
    int filter_size() const;

    void print_taps();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_FLL_BAND_EDGE_CC_IMPL_H */
