/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H
#define INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H

#include "device_source_impl.h"
#include <gnuradio/iio/fmcomms2_source.h>

#include <string>
#include <thread>
#include <vector>

#include <volk/volk_alloc.hh>

namespace gr {
namespace iio {

template <typename T>
class fmcomms2_source_impl : public fmcomms2_source<T>, public device_source_impl
{
private:
    std::vector<std::string>
    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);
    std::vector<std::string> get_channels_vector(const std::vector<bool>& ch_en);
    std::thread overflow_thd;
    void check_overflow(void);

    const static int s_initial_device_buf_size = 8192;
    std::vector<volk::vector<short>> d_device_bufs;
    gr_vector_void_star d_device_item_ptrs;
    volk::vector<float> d_float_rvec;
    volk::vector<float> d_float_ivec;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    bool start() override;
    bool stop() override;

public:
    fmcomms2_source_impl(iio_context* ctx,
                         const std::vector<bool>& ch_en,
                         unsigned long buffer_size);

    ~fmcomms2_source_impl();

    void set_len_tag_key(const std::string& len_tag_key) override;
    void set_frequency(double frequency) override;
    void set_samplerate(unsigned long samplerate) override;
    void set_gain_mode(size_t chan, const std::string& mode) override;
    void set_gain(size_t chan, double gain_value) override;
    void set_quadrature(bool quadrature) override;
    void set_rfdc(bool rfdc) override;
    void set_bbdc(bool bbdc) override;
    void set_filter_params(const std::string& filter_source,
                           const std::string& filter_filename,
                           float fpass,
                           float fstop) override;

protected:
    void update_dependent_params();

    unsigned long long d_frequency = 2400000000;
    unsigned long d_samplerate = 1000000;
    unsigned long d_bandwidth = 20000000;
    bool d_quadrature = true;
    bool d_rfdc = true;
    bool d_bbdc = true;
    std::vector<std::string> d_gain_mode = {
        "manual", "manual", "manual", "manual"
    }; // TODO - make these enums
    std::vector<double> d_gain_value = { 0, 0, 0, 0 };
    std::string d_rf_port_select = "A_BALANCED";
    std::string d_filter_source = "Auto";
    std::string d_filter_filename = "";
    float d_fpass = (float)d_samplerate / 4.0;
    float d_fstop = (float)d_samplerate / 3.0;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H */
