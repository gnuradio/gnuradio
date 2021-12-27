/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H
#define INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H

#include "device_sink_impl.h"
#include <gnuradio/iio/fmcomms2_sink.h>

#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <volk/volk_alloc.hh>

namespace gr {
namespace iio {

template <class T>
class fmcomms2_sink_impl : public fmcomms2_sink<T>, public device_sink_impl
{
private:
    bool cyclic, stop_thread;

    std::mutex uf_mutex;
    std::thread underflow_thd;
    std::vector<std::string>

    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);
    std::vector<std::string> get_channels_vector(const std::vector<bool>& ch_en);
    void check_underflow(void);

    const static int s_initial_device_buf_size = 8192;

    std::vector<volk::vector<short>> d_device_bufs;
    gr_vector_const_void_star d_device_item_ptrs;
    volk::vector<float> d_float_r;
    volk::vector<float> d_float_i;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

public:
    fmcomms2_sink_impl(iio_context* ctx,
                       const std::vector<bool>& ch_en,
                       unsigned long buffer_size,
                       bool cyclic);

    ~fmcomms2_sink_impl() override;

    void update_dependent_params();
    void set_len_tag_key(const std::string& len_tag_key) override;
    void set_bandwidth(unsigned long bandwidth) override;
    void set_rf_port_select(const std::string& rf_port_select) override;
    void set_frequency(double frequency) override;
    void set_samplerate(unsigned long samplerate) override;
    void set_attenuation(size_t chan, double gain) override;
    void set_filter_params(const std::string& filter_source,
                           const std::string& filter_filename = "",
                           float fpass = 0.0,
                           float fstop = 0.0) override;

protected:
    unsigned long long d_frequency = 2400000000;
    unsigned long d_samplerate = 1000000;
    unsigned long d_bandwidth = 20000000;

    std::vector<double> d_attenuation = { 50.0, 50.0, 50.0, 50.0 };
    std::string d_rf_port_select = "A";
    std::string d_filter_source = "Auto";
    std::string d_filter_filename = "";
    float d_fpass = (float)d_samplerate / 4.0;
    float d_fstop = (float)d_samplerate / 3.0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H */
