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
