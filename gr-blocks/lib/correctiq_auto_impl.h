/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_IMPL_H
#define INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_IMPL_H

#include <gnuradio/blocks/correctiq_auto.h>

namespace gr {
namespace blocks {

class correctiq_auto_impl : public correctiq_auto
{
private:
    float d_avg_real;
    float d_avg_img;
    float d_ratio;
    gr_complex d_k;

    double d_samp_rate;
    double d_freq;
    float d_gain;
    float d_sync_window;

    long d_sync_counter;
    bool d_synchronized;

    long d_max_sync_samples;

    void send_sync_values();
    void send_syncing();

    void trigger_resync(std::string reason);

    int d_buffer_size;
    gr_complex* d_volk_const_buffer;

    void set_const_buffer(int new_size);
    void fill_const_buffer();

public:
    correctiq_auto_impl(double samp_rate, double freq, float gain, float sync_window);
    ~correctiq_auto_impl();

    virtual double get_freq();
    virtual float get_gain();

    virtual void set_freq(double new_value);
    virtual void set_gain(float new_value);

    void handle_resync(pmt::pmt_t msg);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_CORRECTIQ_CORRECTIQ_AUTO_IMPL_H */
