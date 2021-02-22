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

#include <string>
#include <vector>

#include <gnuradio/iio/fmcomms2_source.h>

#include "device_source_impl.h"

namespace gr {
namespace iio {

class fmcomms2_source_impl : public fmcomms2_source, public device_source_impl
{
private:
    std::vector<std::string>
    get_channels_vector(bool ch1_en, bool ch2_en, bool ch3_en, bool ch4_en);
    boost::thread overflow_thd;
    void check_overflow(void);

public:
    fmcomms2_source_impl(struct iio_context* ctx,
                         bool destroy_ctx,
                         unsigned long long frequency,
                         unsigned long samplerate,
                         unsigned long bandwidth,
                         bool ch1_en,
                         bool ch2_en,
                         bool ch3_en,
                         bool ch4_en,
                         unsigned long buffer_size,
                         bool quadrature,
                         bool rfdc,
                         bool bbdc,
                         const char* gain1,
                         double gain1_value,
                         const char* gain2,
                         double gain2_value,
                         const char* rf_port_select,
                         const char* filter_source,
                         const char* filter_filename,
                         float Fpass,
                         float Fstop);

    ~fmcomms2_source_impl();

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    bool quadrature,
                    bool rfdc,
                    bool bbdc,
                    const char* gain1,
                    double gain1_value,
                    const char* gain2,
                    double gain2_value,
                    const char* rf_port_select,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_FMCOMMS2_SOURCE_IMPL_H */
