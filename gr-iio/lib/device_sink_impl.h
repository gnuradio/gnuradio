/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_DEVICE_SINK_IMPL_H
#define INCLUDED_IIO_DEVICE_SINK_IMPL_H

#include <gnuradio/iio/device_sink.h>
#include <iio.h>

#include <string>
#include <vector>

namespace gr {
namespace iio {

class device_sink_impl : public device_sink
{
private:
    void channel_write(const struct iio_channel* chn, const void* src, size_t len);
    std::vector<tag_t> d_tags;

protected:
    struct iio_context* ctx;
    struct iio_device *dev, *phy;
    struct iio_buffer* buf;
    std::vector<struct iio_channel*> channel_list;
    unsigned int interpolation;
    unsigned int buffer_size;
    bool destroy_ctx;
    pmt::pmt_t d_len_tag_key;

public:
    device_sink_impl(struct iio_context* ctx,
                     bool destroy_ctx,
                     const std::string& device,
                     const std::vector<std::string>& channels,
                     const std::string& device_phy,
                     const std::vector<std::string>& params,
                     unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                     unsigned int interpolation = 0,
                     bool cyclic = false);

    ~device_sink_impl();

    void set_params(const std::vector<std::string>& params);

    void set_len_tag_key(const std::string& len_tag_key) override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SINK_IMPL_H */
