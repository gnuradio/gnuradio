/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_PLUTO_SOURCE_IMPL_H
#define INCLUDED_IIO_PLUTO_SOURCE_IMPL_H

#include <string>
#include <vector>

#include <gnuradio/iio/fmcomms2_source.h>
#include <gnuradio/iio/pluto_source.h>

#include "device_source_impl.h"

namespace gr {
namespace iio {

class pluto_source_impl : public pluto_source, public fmcomms2_source_f32c
{
private:
    // void update_rate(unsigned long samplerate)

public:
    explicit pluto_source_impl(fmcomms2_source::sptr block);

    static std::string get_uri();

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    bool quadrature,
                    bool rfdc,
                    bool bbdc,
                    const char* gain,
                    double gain_value,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_PLUTO_SOURCE_IMPL_H */
