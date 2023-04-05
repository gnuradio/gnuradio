/* -*- c++ -*- */
/*
 * Copyright 2023 Ettus Research, A National Instruments Brand
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H
#define INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H

#include <gnuradio/blocks/complex_to_mag_squared.h>
#include <gnuradio/blocks/float_to_uchar.h>
#include <gnuradio/blocks/keep_one_in_n.h>
#include <gnuradio/blocks/nlog10_ff.h>
#include <gnuradio/blocks/stream_to_vector.h>
#include <gnuradio/fft/fft_v.h>
#include <gnuradio/filter/single_pole_iir_filter_ff.h>
#include <gnuradio/qtgui/fosphor_formatter.h>
#include <volk/volk_alloc.hh>

namespace gr {
namespace qtgui {

class fosphor_formatter_impl : public fosphor_formatter
{
public:
    // Helper block to convert float->uchar *with* vector length
    class f2uchar_vfvb : virtual public gr::sync_block
    {
    public:
        typedef std::shared_ptr<f2uchar_vfvb> sptr;
    };

    // Helper block to do the actual histogram processing
    class histo_proc : virtual public gr::block
    {
    public:
        typedef std::shared_ptr<histo_proc> sptr;
    };

    fosphor_formatter_impl(int fft_size,
                           int num_bins,
                           int input_decim,
                           int waterfall_decim,
                           int histo_decim,
                           double scale,
                           double alpha,
                           double epsilon,
                           double trise,
                           double tdecay);

    ~fosphor_formatter_impl() override = default;

private:
    // All the internal blocks
    gr::blocks::stream_to_vector::sptr d_s2v;
    gr::blocks::keep_one_in_n::sptr d_input_decim;
    gr::fft::fft_v<gr_complex, true>::sptr d_fft;
    gr::blocks::complex_to_mag_squared::sptr d_c2m;
    gr::blocks::nlog10_ff::sptr d_log;
    f2uchar_vfvb::sptr d_f2byte; // float_to_uchar does not support vectors
    gr::filter::single_pole_iir_filter_ff::sptr d_avg;
    histo_proc::sptr d_histo_proc;
    gr::blocks::keep_one_in_n::sptr d_wf_decim;
};

} // namespace qtgui
} /* namespace gr */

#endif /* INCLUDED_QTGUI_FOSPHOR_FORMATTER_IMPL_H */
