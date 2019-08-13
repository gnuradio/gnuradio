/* -*- c++ -*- */
/*
 * Copyright 2017 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "pluto_sink_impl.h"
#include "pluto_source_impl.h"

#include <iio.h>

namespace gr {
namespace iio {

pluto_sink::sptr pluto_sink::make(const std::string& uri,
                                  unsigned long long frequency,
                                  unsigned long samplerate,
                                  unsigned long bandwidth,
                                  unsigned long buffer_size,
                                  bool cyclic,
                                  double attenuation,
                                  const char* filter_source,
                                  const char* filter_filename,
                                  float Fpass,
                                  float Fstop)
{
    fmcomms2_sink::sptr block =
        fmcomms2_sink::make(uri.empty() ? pluto_source_impl::get_uri() : uri,
                            frequency,
                            samplerate,
                            bandwidth,
                            true,
                            true,
                            false,
                            false,
                            buffer_size,
                            cyclic,
                            "A",
                            attenuation,
                            0.0,
                            filter_source,
                            filter_filename,
                            Fpass,
                            Fstop);

    return gnuradio::get_initial_sptr(new pluto_sink_impl(block));
}

pluto_sink_impl::pluto_sink_impl(fmcomms2_sink::sptr block)
    : hier_block2("pluto_sink",
                  io_signature::make(1, 1, sizeof(gr_complex)),
                  io_signature::make(0, 0, 0)),
      fmcomms2_sink_f32c(true, false, block)
{
}

void pluto_sink_impl::set_params(unsigned long long frequency,
                                 unsigned long samplerate,
                                 unsigned long bandwidth,
                                 double attenuation,
                                 const char* filter_source,
                                 const char* filter_filename,
                                 float Fpass,
                                 float Fstop)
{
    fmcomms2_sink_f32c::set_params(frequency,
                                   samplerate,
                                   bandwidth,
                                   "A",
                                   attenuation,
                                   0.0,
                                   filter_source,
                                   filter_filename,
                                   Fpass,
                                   Fstop);
}

} // namespace iio
} // namespace gr
