/* -*- c++ -*- */
/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_SOURCE_IMPL_H
#define INCLUDED_IIO_AD9081_SOURCE_IMPL_H

#include "ad9081_common_impl.h"
#include "device_source_impl.h"

#include <gnuradio/iio/ad9081_source.h>

#include <array>
#include <string>
#include <vector>

namespace gr {
namespace iio {

class ad9081_source_impl : public ad9081_source,
                           public device_source_impl,
                           public ad9081_common_impl
{
private:
public:
    ad9081_source_impl(struct iio_context* ctx,
                       std::array<bool, MAX_CHANNEL_COUNT> en,
                       size_t buffer_size);

    ~ad9081_source_impl() = default;


    /*!
     * \brief Set nyquist zone.
     */
    void set_nyquist_zone(bool odd) override
    {
        auto& st = d_channel_state[d_last_active_channel];
        int ret = iio_channel_attr_write(st.ch_i, "nyquist_zone", odd ? "odd" : "even");
        if (ret < 0)
            throw std::runtime_error(
                "ad9081: set_nyquist_zone: Failed to write NCO attribute");
    }

    /*!
     * \brief Update programmable filter
     */
    void set_filter_source_file(const std::string& src_file) override;
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SOURCE_IMPL_H */
