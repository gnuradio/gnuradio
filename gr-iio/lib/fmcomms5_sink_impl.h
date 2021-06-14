/* -*- c++ -*- */
/*
 * Copyright 2015 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS5_SINK_IMPL_H
#define INCLUDED_IIO_FMCOMMS5_SINK_IMPL_H

#include "device_sink_impl.h"
#include <gnuradio/iio/fmcomms5_sink.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

class fmcomms5_sink_impl : public fmcomms5_sink, public device_sink_impl
{
private:
    bool cyclic;
    unsigned long samplerate;
    struct iio_device* phy2;

    static void set_params(struct iio_device* phy_device,
                           unsigned long long frequency,
                           unsigned long samplerate,
                           unsigned long bandwidth,
                           const char* rf_port_select,
                           double attenuation1,
                           double attenuation2,
                           const char* filter_source,
                           const char* filter_filename,
                           float Fpass,
                           float Fstop);

    std::vector<std::string> get_channels_vector(bool ch1_en,
                                                 bool ch2_en,
                                                 bool ch3_en,
                                                 bool ch4_en,
                                                 bool ch5_en,
                                                 bool ch6_en,
                                                 bool ch7_en,
                                                 bool ch8_en);

public:
    fmcomms5_sink_impl(struct iio_context* ctx,
                       bool destroy_ctx,
                       unsigned long long frequency1,
                       unsigned long long frequency2,
                       unsigned long samplerate,
                       unsigned long bandwidth,
                       bool ch1_en,
                       bool ch2_en,
                       bool ch3_en,
                       bool ch4_en,
                       bool ch5_en,
                       bool ch6_en,
                       bool ch7_en,
                       bool ch8_en,
                       unsigned long buffer_size,
                       bool cyclic,
                       const char* rf_port_select,
                       double attenuation1,
                       double attenuation2,
                       double attenuation3,
                       double attenuation4,
                       const char* filter_source,
                       const char* filter_filename,
                       float Fpass,
                       float Fstop);

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void set_params(unsigned long long frequency1,
                    unsigned long long frequency2,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    const char* rf_port_select,
                    double attenuation1,
                    double attenuation2,
                    double attenuation3,
                    double attenuation4,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SINK_IMPL_H */
