/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "pluto_sink_impl.h"
#include "pluto_source_impl.h"
#include <iio.h>

#include <string>

namespace gr {
namespace iio {

pluto_sink::sptr
pluto_sink::make(const std::string& uri, unsigned long buffer_size, bool cyclic)
{
    return gnuradio::get_initial_sptr(
        new pluto_sink_impl(uri.empty() ? pluto_source_impl::get_uri() : uri,
                            buffer_size,
                            cyclic));
}

pluto_sink_impl::pluto_sink_impl(const std::string& uri,
                                 unsigned long buffer_size,
                                 bool cyclic)
    : fmcomms2_sink_impl(device_source_impl::get_context(uri), {true, false, false, false}, buffer_size, cyclic)
{
}

void pluto_sink_impl::set_len_tag_key(const std::string& len_tag_key)
{
    fmcomms2_sink_impl::set_len_tag_key(len_tag_key);
}
void pluto_sink_impl::set_frequency(unsigned long long frequency)
{
    fmcomms2_sink_impl::set_frequency(frequency);
}
void pluto_sink_impl::set_bandwidth(unsigned long bandwidth)
{
    fmcomms2_sink_impl::set_bandwidth(bandwidth);
}
void pluto_sink_impl::set_samplerate(unsigned long samplerate)
{
    fmcomms2_sink_impl::set_samplerate(samplerate);
}
void pluto_sink_impl::set_attenuation(double attenuation)
{
    fmcomms2_sink_impl::set_attenuation(0, attenuation);
}
void pluto_sink_impl::set_attenuation(size_t chan, double attenuation)
{
    fmcomms2_sink_impl::set_attenuation(chan, attenuation);
}

void pluto_sink_impl::set_filter_params(const std::string& filter_source,
                                        const std::string& filter_filename,
                                        float fpass,
                                        float fstop)
{
    fmcomms2_sink_impl::set_filter_params(filter_source, filter_filename, fpass, fstop);
}

void pluto_sink_impl::set_rf_port_select(const std::string& rf_port_select)
{
    fmcomms2_sink_impl::set_rf_port_select(rf_port_select);
}


} // namespace iio
} // namespace gr
