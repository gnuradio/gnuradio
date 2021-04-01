/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "device_sink_impl.h"
#include "device_source_impl.h"
#include <gnuradio/io_signature.h>

#include <cstdio>

namespace gr {
namespace iio {

device_sink::sptr device_sink::make(const std::string& uri,
                                    const std::string& device,
                                    const std::vector<std::string>& channels,
                                    const std::string& device_phy,
                                    const std::vector<std::string>& params,
                                    unsigned int buffer_size,
                                    unsigned int interpolation,
                                    bool cyclic)
{
    return gnuradio::get_initial_sptr(
        new device_sink_impl(device_source_impl::get_context(uri),
                             true,
                             device,
                             channels,
                             device_phy,
                             params,
                             buffer_size,
                             interpolation,
                             cyclic));
}

device_sink::sptr device_sink::make_from(struct iio_context* ctx,
                                         const std::string& device,
                                         const std::vector<std::string>& channels,
                                         const std::string& device_phy,
                                         const std::vector<std::string>& params,
                                         unsigned int buffer_size,
                                         unsigned int interpolation,
                                         bool cyclic)
{
    return gnuradio::get_initial_sptr(new device_sink_impl(ctx,
                                                           false,
                                                           device,
                                                           channels,
                                                           device_phy,
                                                           params,
                                                           buffer_size,
                                                           interpolation,
                                                           cyclic));
}

void device_sink_impl::set_params(const std::vector<std::string>& params)
{
    device_source_impl::set_params(this->phy, params);
}

/*
 * The private constructor
 */
device_sink_impl::device_sink_impl(struct iio_context* ctx,
                                   bool destroy_ctx,
                                   const std::string& device,
                                   const std::vector<std::string>& channels,
                                   const std::string& device_phy,
                                   const std::vector<std::string>& params,
                                   unsigned int buffer_size,
                                   unsigned int interpolation,
                                   bool cyclic)
    : gr::sync_block("device_sink",
                     gr::io_signature::make(1, -1, sizeof(short)),
                     gr::io_signature::make(0, 0, 0)),
      ctx(ctx),
      interpolation(interpolation),
      buffer_size(buffer_size),
      destroy_ctx(destroy_ctx)
{
    unsigned int nb_channels, i;

    /* Set minimum input size */
    set_output_multiple(buffer_size / (interpolation + 1));

    if (!ctx)
        throw std::runtime_error("Unable to create context");

    dev = iio_context_find_device(ctx, device.c_str());
    phy = iio_context_find_device(ctx, device_phy.c_str());
    if (!dev || !phy) {
        if (destroy_ctx)
            iio_context_destroy(ctx);
        throw std::runtime_error("Device not found");
    }

    /* First disable all channels */
    nb_channels = iio_device_get_channels_count(dev);
    for (i = 0; i < nb_channels; i++)
        iio_channel_disable(iio_device_get_channel(dev, i));

    if (channels.empty()) {
        for (i = 0; i < nb_channels; i++) {
            struct iio_channel* chn = iio_device_get_channel(dev, i);

            iio_channel_enable(chn);
            channel_list.push_back(chn);
        }
    } else {
        for (std::vector<std::string>::const_iterator it = channels.begin();
             it != channels.end();
             ++it) {
            struct iio_channel* chn = iio_device_find_channel(dev, it->c_str(), true);
            if (!chn) {
                if (destroy_ctx)
                    iio_context_destroy(ctx);
                throw std::runtime_error("Channel not found");
            }

            iio_channel_enable(chn);
            if (!iio_channel_is_enabled(chn))
                throw std::runtime_error("Channel not enabled");
            channel_list.push_back(chn);
        }
    }

    set_params(params);

    buf = iio_device_create_buffer(dev, buffer_size, cyclic);
    if (!buf)
        throw std::runtime_error("Unable to create buffer: " + boost::to_string(-errno));
}

/*
 * Our virtual destructor.
 */
device_sink_impl::~device_sink_impl()
{
    iio_buffer_destroy(buf);
    device_source_impl::remove_ctx_history(ctx, destroy_ctx);
}

void device_sink_impl::channel_write(const struct iio_channel* chn,
                                     const void* src,
                                     size_t len)
{
    uintptr_t dst_ptr, src_ptr = (uintptr_t)src, end = src_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t)iio_buffer_end(buf);
    ptrdiff_t buf_step = iio_buffer_step(buf) * (interpolation + 1);

    for (dst_ptr = (uintptr_t)iio_buffer_first(buf, chn);
         dst_ptr < buf_end && src_ptr + length <= end;
         dst_ptr += buf_step, src_ptr += length)
        iio_channel_convert_inverse(chn, (void*)dst_ptr, (const void*)src_ptr);
}

int device_sink_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    int ret;

    if (interpolation >= 1) {
        ptrdiff_t len = (intptr_t)iio_buffer_end(buf) - (intptr_t)iio_buffer_start(buf);
        memset(iio_buffer_start(buf), 0, len);
    }

    for (unsigned int i = 0; i < input_items.size(); i++)
        channel_write(channel_list[i], input_items[i], noutput_items * sizeof(short));

    ret = iio_buffer_push(buf);
    if (ret < 0) {
        char buf[256];
        iio_strerror(-ret, buf, sizeof(buf));
        std::string error(buf);

        std::cerr << "Unable to push buffer: " << error << std::endl;
        return -1; /* EOF */
    }

    consume_each(buffer_size / (interpolation + 1));
    return 0;
}

void device_sink_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    for (unsigned int i = 0; i < ninput_items_required.size(); i++)
        ninput_items_required[i] = noutput_items;
}

} /* namespace iio */
} /* namespace gr */
