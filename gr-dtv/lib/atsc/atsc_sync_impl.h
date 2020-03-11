/* -*- c++ -*- */
/*
 * Copyright 2014,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_ATSC_SYNC_IMPL_H
#define INCLUDED_DTV_ATSC_SYNC_IMPL_H

#include <gnuradio/dtv/atsc_consts.h>
#include <gnuradio/dtv/atsc_sync.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>
#include <gnuradio/filter/single_pole_iir.h>

namespace gr {
namespace dtv {

class atsc_sync_impl : public atsc_sync
{
private:
    gr::filter::single_pole_iir<float, float, float> d_loop; // ``VCO'' loop filter
    gr::filter::mmse_fir_interpolator_ff d_interp;

    double d_rx_clock_to_symbol_freq;
    int d_si;
    double d_w;  // ratio of PERIOD of Tx to Rx clocks
    double d_mu; // fractional delay [0,1]
    int d_incr;

    float d_sample_mem[ATSC_DATA_SEGMENT_LENGTH];
    float d_data_mem[ATSC_DATA_SEGMENT_LENGTH];

    double d_timing_adjust;
    int d_counter; // free running mod 832 counter
    int d_symbol_index;
    bool d_seg_locked;
    unsigned char d_sr; // 4 bit shift register
    signed char d_integrator[ATSC_DATA_SEGMENT_LENGTH];
    int d_output_produced;

public:
    atsc_sync_impl(float rate);
    ~atsc_sync_impl();

    void reset();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    virtual int general_work(int noutput_items,
                             gr_vector_int& ninput_items,
                             gr_vector_const_void_star& input_items,
                             gr_vector_void_star& output_items);
};

} /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_SYNC_IMPL_H */
