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

#include <string>
#include <vector>

#include <gnuradio/iio/fmcomms2_sink.h>

#include "device_sink_impl.h"

namespace gr {
namespace iio {

class fmcomms2_sink_impl : public fmcomms2_sink, public device_sink_impl
{
private:
    bool cyclic, stop_thread;
    boost::mutex uf_mutex;
    boost::thread underflow_thd;

    std::vector<std::string>
    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);

    void check_underflow(void);

public:
    fmcomms2_sink_impl(struct iio_context* ctx,
                       bool destroy_ctx,
                       unsigned long long frequency,
                       unsigned long samplerate,
                       unsigned long bandwidth,
                       bool ch1_en,
                       bool ch2_en,
                       bool ch3_en,
                       bool ch4_en,
                       unsigned long buffer_size,
                       bool cyclic,
                       const char* rf_port_select,
                       double attenuation1,
                       double attenuation2,
                       const char* filter_source,
                       const char* filter_filename,
                       float Fpass,
                       float Fstop);

    ~fmcomms2_sink_impl();

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    const char* rf_port_select,
                    double attenuation1,
                    double attenuation2,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H */
