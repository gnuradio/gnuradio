/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_PLUTO_SINK_IMPL_H
#define INCLUDED_IIO_PLUTO_SINK_IMPL_H

#include <string>
#include <vector>

#include <gnuradio/iio/fmcomms2_sink.h>
#include <gnuradio/iio/pluto_sink.h>

#include "device_sink_impl.h"

namespace gr {
namespace iio {

class pluto_sink_impl : public pluto_sink, public fmcomms2_sink_f32c
{
public:
    explicit pluto_sink_impl(fmcomms2_sink::sptr block);

    void set_params(unsigned long long frequency,
                    unsigned long samplerate,
                    unsigned long bandwidth,
                    double attenuation,
                    const char* filter_source,
                    const char* filter_filename,
                    float Fpass,
                    float Fstop);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_PLUTO_SINK_IMPL_H */
