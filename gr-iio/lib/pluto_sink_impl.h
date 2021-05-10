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

#include "device_sink_impl.h"
#include <gnuradio/iio/fmcomms2_sink.h>
#include <gnuradio/iio/pluto_sink.h>

namespace gr {
namespace iio {

class pluto_sink_impl : public pluto_sink, public fmcomms2_sink_f32c
{
public:
    explicit pluto_sink_impl(fmcomms2_sink::sptr block);

    virtual void set_frequency(unsigned long long frequency);
    virtual void set_bandwidth(unsigned long bandwidth);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_attenuation(double attenuation);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);

};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_PLUTO_SINK_IMPL_H */
