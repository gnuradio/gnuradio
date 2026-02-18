/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_ATTR_SINK_IMPL_H
#define INCLUDED_IIO_ATTR_SINK_IMPL_H

#include "device_source_impl.h"
#include <gnuradio/iio/attr_sink.h>
#include <pmt/pmt.h>
#include <string>

#ifdef LIBIIO_V1
#include <iio/iio.h>
#else
#include <iio.h>
#endif

namespace gr {
namespace iio {

class attr_sink_impl : public attr_sink
{
private:
    std::string device;
    std::string channel;
    std::string uri;
    attr_type_t type;

protected:
    iio_context* ctx;
    iio_device* dev;
    iio_channel* chan;

public:
    attr_sink_impl(const std::string& uri,
                   const std::string& device,
                   const std::string& channel,
                   attr_type_t type,
                   bool output);
    ~attr_sink_impl();

    void write_attribute(pmt::pmt_t pdu);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SINK_IMPL_H */
