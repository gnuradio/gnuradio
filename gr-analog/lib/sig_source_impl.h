/* -*- c++ -*- */
/*
 * Copyright 2004,2012,2018,2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef SIG_SOURCE_IMPL_H
#define SIG_SOURCE_IMPL_H

#include <gnuradio/analog/sig_source.h>
#include <gnuradio/fxpt_nco.h>
#include <gnuradio/sync_block.h>

namespace gr {
namespace analog {

template <class T>
class sig_source_impl : public sig_source<T>
{
private:
    double d_sampling_freq;
    gr_waveform_t d_waveform;
    double d_frequency;
    double d_ampl;
    T d_offset;
    gr::fxpt_nco d_nco;

public:
    sig_source_impl(double sampling_freq,
                    gr_waveform_t waveform,
                    double wave_freq,
                    double ampl,
                    T offset = 0,
                    float phase = 0);
    ~sig_source_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    double sampling_freq() const override { return d_sampling_freq; }
    gr_waveform_t waveform() const override { return d_waveform; }
    double frequency() const override { return d_frequency; }
    double amplitude() const override { return d_ampl; }
    T offset() const override { return d_offset; }
    float phase() const override { return d_nco.get_phase(); }

    void set_sampling_freq(double sampling_freq) override;
    void set_waveform(gr_waveform_t waveform) override;

    // Message handlers
    void set_cmd_msg(pmt::pmt_t msg);

    void set_frequency(double frequency) override;
    void set_amplitude(double ampl) override;
    void set_offset(T offset) override;
    void set_phase(float phase) override;
};

} /* namespace analog */
} /* namespace gr */

#endif /* SIG_SOURCE_IMPL_H */
