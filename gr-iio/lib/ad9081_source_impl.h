/* -*- c++ -*- */
/*
 * Copyright 2021 Analog Devices, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_AD9081_SOURCE_IMPL_H
#define INCLUDED_IIO_AD9081_SOURCE_IMPL_H

#include "ad9081_common.h"
#include "device_source_impl.h"

#include <gnuradio/iio/ad9081_source.h>

#include <array>
#include <string>
#include <vector>

namespace gr {
namespace iio {

class ad9081_source_impl : public ad9081_source,
                           public device_source_impl,
                           public ad9081_common
{
private:
    void channel_read(const struct iio_channel* chn, void* dst, size_t len);

public:
    static constexpr int MAX_CHANNEL_COUNT = ad9081_source::MAX_CHANNEL_COUNT;

    ad9081_source_impl(struct iio_context* ctx,
                       const std::array<bool, MAX_CHANNEL_COUNT>& en,
                       size_t buffer_size);

    ~ad9081_source_impl() = default;

    virtual int work(int noutput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items) override;

    void set_main_nco_freq(int nco, double freq);

    void set_main_nco_phase(int nco, float phase);

    void set_channel_nco_freq(int nco, double freq);

    void set_channel_nco_phase(int nco, float phase);

    void set_nyquist_zone(bool odd);

    void set_filter_source_file(const std::string& src_file);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_AD9081_SOURCE_IMPL_H */
