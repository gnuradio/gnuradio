/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H
#define INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H

#include "device_source_impl.h"
#include <gnuradio/iio/fmcomms5_source.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

class fmcomms5_source_impl : public fmcomms5_source, public device_source_impl
{
private:
    unsigned long samplerate;
    struct iio_device* phy2;

    static void set_params(struct iio_device* phy_device,
                           unsigned long long frequency,
                           unsigned long samplerate,
                           unsigned long bandwidth,
                           bool quadrature,
                           bool rfdc,
                           bool bbdc,
                           const char* gain1,
                           double gain1_value,
                           const char* gain2,
                           double gain2_value,
                           const char* port_select,
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
    fmcomms5_source_impl(struct iio_context* ctx,
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
                         bool quadrature,
                         bool rfdc,
                         bool bbdc,
                         const char* gain1,
                         double gain1_value,
                         const char* gain2,
                         double gain2_value,
                         const char* gain3,
                         double gain3_value,
                         const char* gain4,
                         double gain4_value,
                         const char* rf_port_select,
                         const char* filter_source,
                         const char* filter_filename,
                         float Fpass,
                         float Fstop);

    void set_params(unsigned long long frequency1,
                    unsigned long long frequency2,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    bool quadrature,
                    bool rfdc,
                    bool bbdc,
                    const char* gain1,
                    double gain1_value,
                    const char* gain2,
                    double gain2_value,
                    const char* gain3,
                    double gain3_value,
                    const char* gain4,
                    double gain4_value,
                    const char* rf_port_select,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS5_SOURCE_IMPL_H */
