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
