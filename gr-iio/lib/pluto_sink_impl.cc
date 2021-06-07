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
    fmcomms2_sink::sptr block =
        fmcomms2_sink::make(uri.empty() ? pluto_source_impl::get_uri() : uri,
                            { true, true, false, false },
                            buffer_size,
                            cyclic);

    return gnuradio::get_initial_sptr(new pluto_sink_impl(block));
}

pluto_sink_impl::pluto_sink_impl(fmcomms2_sink::sptr block)
    : hier_block2("pluto_sink",
                  io_signature::make(1, 1, sizeof(gr_complex)),
                  io_signature::make(0, 0, 0)),
      fmcomms2_sink_f32c(true, false, block)
{
}

void pluto_sink_impl::set_frequency(unsigned long long frequency)
{
    fmcomms2_sink_f32c::set_frequency(frequency);
}
void pluto_sink_impl::set_bandwidth(unsigned long bandwidth)
{
    fmcomms2_sink_f32c::set_bandwidth(bandwidth);
}
void pluto_sink_impl::set_samplerate(unsigned long samplerate)
{
    fmcomms2_sink_f32c::set_samplerate(samplerate);
}
void pluto_sink_impl::set_attenuation(double attenuation)
{
    fmcomms2_sink_f32c::set_attenuation(0, attenuation);
}
void pluto_sink_impl::set_filter_params(const std::string& filter_source,
                                        const std::string& filter_filename,
                                        float fpass,
                                        float fstop)
{
    fmcomms2_sink_f32c::set_filter_params(filter_source, filter_filename, fpass, fstop);
}


} // namespace iio
} // namespace gr
