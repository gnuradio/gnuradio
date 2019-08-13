/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
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

#ifndef INCLUDED_IIO_ATTR_SINK_IMPL_H
#define INCLUDED_IIO_ATTR_SINK_IMPL_H

#include <gnuradio/iio/attr_sink.h>
#include <iio.h>
#include <pmt/pmt.h>

#include "device_source_impl.h"

namespace gr {
namespace iio {

class attr_sink_impl : public attr_sink
{
private:
    std::string device;
    std::string channel;
    std::string uri;
    int type;
    bool output;
    bool required_enable;

protected:
    struct iio_context* ctx;
    struct iio_device* dev;
    struct iio_channel* chan;

public:
    attr_sink_impl(const std::string& uri,
                   const std::string& device,
                   const std::string& channel,
                   int type,
                   bool output,
                   bool required_enable);
    ~attr_sink_impl();

    void write_attribute(pmt::pmt_t pdu);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SINK_IMPL_H */
