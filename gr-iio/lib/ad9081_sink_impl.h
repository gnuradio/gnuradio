/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_SINK_IMPL_H
#define INCLUDED_IIO_AD9081_SINK_IMPL_H

#include "ad9081_common_impl.h"
#include "device_sink_impl.h"

#include <gnuradio/iio/ad9081_sink.h>

#include <array>
#include <string>
#include <vector>

namespace gr {
namespace iio {

class ad9081_sink_impl : public ad9081_sink,
                         public device_sink_impl,
                         public ad9081_common_impl
{
private:
public:
    ad9081_sink_impl(struct iio_context* ctx,
                     std::array<bool, MAX_CHANNEL_COUNT> en,
                     size_t buffer_size,
                     bool cyclic = false);

    ~ad9081_sink_impl() = default;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SINK_IMPL_H */
