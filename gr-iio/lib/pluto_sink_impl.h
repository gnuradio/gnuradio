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
#include "fmcomms2_sink_impl.h"
#include <gnuradio/iio/pluto_sink.h>

namespace gr {
namespace iio {

class pluto_sink_impl : public pluto_sink, virtual public fmcomms2_sink_impl<gr_complex>
{
public:
    pluto_sink_impl(const std::string& uri, unsigned long buffer_size, bool cyclic);

    virtual void set_rf_port_select(const std::string& rf_port_select);
    virtual void set_len_tag_key(const std::string& len_tag_key);
    virtual void set_frequency(unsigned long long frequency);
    virtual void set_bandwidth(unsigned long bandwidth);
    virtual void set_samplerate(unsigned long samplerate);
    virtual void set_attenuation(double attenuation);
    virtual void set_attenuation(size_t chan, double attenuation);
    virtual void set_filter_params(const std::string& filter_source,
                                   const std::string& filter_filename = "",
                                   float fpass = 0.0,
                                   float fstop = 0.0);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_PLUTO_SINK_IMPL_H */
