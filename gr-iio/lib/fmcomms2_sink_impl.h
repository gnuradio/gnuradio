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

namespace gr {
namespace iio {

class fmcomms2_sink_impl : public fmcomms2_sink, public device_sink_impl
{
private:
    bool cyclic, stop_thread;

    std::mutex uf_mutex;
    std::thread underflow_thd;
    std::vector<std::string>

    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);
    std::vector<std::string> get_channels_vector(const std::vector<bool>& ch_en);
    void check_underflow(void);

public:
    fmcomms2_sink_impl(struct iio_context* ctx,
                       const std::vector<bool>& ch_en,
                       unsigned long buffer_size,
                       bool cyclic);

    ~fmcomms2_sink_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void update_dependent_params();
    virtual void set_bandwidth(unsigned long bandwidth);
    virtual void set_rf_port_select(const std::string& rf_port_select);
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_attenuation(size_t chan, double gain);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);

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
