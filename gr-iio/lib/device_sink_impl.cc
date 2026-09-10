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

#include <string>
#include <vector>

namespace gr {
namespace iio {

device_sink::sptr device_sink::make(const std::string& uri,
                                    const std::string& device,
                                    const std::vector<std::string>& channels,
                                    const std::string& device_phy,
                                    const iio_param_vec_t& params,
                                    unsigned int buffer_size,
                                    unsigned int interpolation,
                                    bool cyclic)
{
    return gnuradio::make_block_sptr<device_sink_impl>(
        device_source_impl::get_context(uri),
        true,
        device,
        channels,
        device_phy,
        params,
        buffer_size,
        interpolation,
        cyclic);
}

device_sink::sptr device_sink::make_from(iio_context* ctx,
                                         const std::string& device,
                                         const std::vector<std::string>& channels,
                                         const std::string& device_phy,
                                         const iio_param_vec_t& params,
                                         unsigned int buffer_size,
                                         unsigned int interpolation,
                                         bool cyclic)
{
    return gnuradio::make_block_sptr<device_sink_impl>(ctx,
                                                       false,
                                                       device,
                                                       channels,
                                                       device_phy,
                                                       params,
                                                       buffer_size,
                                                       interpolation,
                                                       cyclic);
}

void device_sink_impl::set_params(const iio_param_vec_t& params)
{
    device_source_impl::set_params(this->phy, params);
}

/*
 * The private constructor
 */
device_sink_impl::device_sink_impl(iio_context* ctx,
                                   bool destroy_ctx,
                                   const std::string& device,
                                   const std::vector<std::string>& channels,
                                   const std::string& device_phy,
                                   const iio_param_vec_t& params,
                                   unsigned int buffer_size,
                                   unsigned int interpolation,
                                   bool cyclic)
    : gr::sync_block("device_sink",
                     gr::io_signature::make(1, -1, sizeof(short)),
                     gr::io_signature::make(0, 0, 0)),
      d_tags(0),
      ctx(ctx),
#ifdef LIBIIO_V1
      buf_stream(NULL),
      cyclic_block(NULL),
      stream_started(false),
#endif
      interpolation(interpolation),
      buffer_size(buffer_size),
      cyclic(cyclic),
      destroy_ctx(destroy_ctx),
      d_len_tag_key(pmt::PMT_NIL)
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
#ifdef LIBIIO_V1
    mask = iio_create_channels_mask(nb_channels);

    if (!mask)
        throw std::runtime_error("Unable to create channel mask.");
#endif

    for (i = 0; i < nb_channels; i++) {
#ifdef LIBIIO_V1
        iio_channel_disable(iio_device_get_channel(dev, i), mask);
#else
        iio_channel_disable(iio_device_get_channel(dev, i));
#endif
    }

    if (channels.empty()) {
        for (i = 0; i < nb_channels; i++) {
            iio_channel* chn = iio_device_get_channel(dev, i);

#ifdef LIBIIO_V1
            iio_channel_enable(chn, mask);
#else
            iio_channel_enable(chn);
#endif
            channel_list.push_back(chn);
        }
    } else {
        for (std::vector<std::string>::const_iterator it = channels.begin();
             it != channels.end();
             ++it) {
            iio_channel* chn = iio_device_find_channel(dev, it->c_str(), true);
            if (!chn) {
                if (destroy_ctx)
                    iio_context_destroy(ctx);
                throw std::runtime_error("Channel not found");
            }

#ifdef LIBIIO_V1
            iio_channel_enable(chn, mask);
            if (!iio_channel_is_enabled(chn, mask))
                throw std::runtime_error("Channel not enabled");
#else
            iio_channel_enable(chn);
            if (!iio_channel_is_enabled(chn))
                throw std::runtime_error("Channel not enabled");
#endif
            channel_list.push_back(chn);
        }
    }

    set_params(params);

#ifdef LIBIIO_V1
    buf = iio_device_get_buffer(dev, 0);
    int err_code = iio_err(buf);
    if (err_code)
        throw std::runtime_error("Unable to create buffer: " + std::to_string(-err_code));

    if (cyclic) {
        /* iio_stream_get_next_block() enqueues with cyclic = false, so a cyclic
         * transfer has to drive a block of its own and pass the flag to
         * iio_block_enqueue() itself. */
        buf_stream = iio_buffer_open(buf, mask);
        err_code = iio_err(buf_stream);
        if (err_code) {
            buf_stream = NULL;
            throw std::runtime_error("Unable to open buffer: " +
                                     std::to_string(-err_code));
        }

        cyclic_block = iio_buffer_stream_create_block(
            buf_stream, (size_t)buffer_size * iio_device_get_sample_size(dev, mask));
        err_code = iio_err(cyclic_block);
        if (err_code) {
            cyclic_block = NULL;
            throw std::runtime_error("Unable to create cyclic block: " +
                                     std::to_string(-err_code));
        }

        /* channel_write() fills whatever iioblock points at. */
        iioblock = cyclic_block;
    } else {
        /* buffer_size is a sample count, which is what create_stream expects. */
        stream = iio_buffer_create_stream(buf, 4, buffer_size, mask);
        err_code = iio_err(stream);
        if (err_code)
            throw std::runtime_error("Unable to create stream: " +
                                     std::to_string(-err_code));

        // get first block so we can copy the data to it
        iioblock = iio_stream_get_next_block(stream);
        err_code = iio_err(iioblock);
        if (err_code)
            throw std::runtime_error("Unable to create first stream block: " +
                                     std::to_string(-err_code));
    }
#else
    buf = iio_device_create_buffer(dev, buffer_size, cyclic);
    if (!buf)
        throw std::runtime_error("Unable to create buffer: " + std::to_string(-errno));
#endif
}

/*
 * Our virtual destructor.
 */
device_sink_impl::~device_sink_impl()
{
#ifdef LIBIIO_V1
    /* The buffer belongs to the device; destroying the stream closes it. */
    if (stream)
        iio_stream_destroy(stream);

    if (buf_stream) {
        /* Cancel first so a transfer in flight is unblocked before the block it
         * uses is destroyed. */
        iio_buffer_stream_cancel(buf_stream);
        if (cyclic_block)
            iio_block_destroy(cyclic_block);
        if (stream_started)
            iio_buffer_stream_stop(buf_stream);
        iio_buffer_close(buf_stream);
    }

    if (mask)
        iio_channels_mask_destroy(mask);
#else
    iio_buffer_destroy(buf);
#endif
    device_source_impl::remove_ctx_history(ctx, destroy_ctx);
}

void device_sink_impl::channel_write(const iio_channel* chn, const void* src, size_t len)
{
#ifdef LIBIIO_V1
    const iio_channels_mask* hw_mask = mask;
    uintptr_t dst_ptr, src_ptr = (uintptr_t)src, end = src_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t)iio_block_end(iioblock);
    ptrdiff_t buf_step = iio_device_get_sample_size(dev, hw_mask) * (interpolation + 1);

    for (dst_ptr = (uintptr_t)iio_block_first(iioblock, chn);
         dst_ptr < buf_end && src_ptr + length <= end;
         dst_ptr += buf_step, src_ptr += length)
        iio_channel_convert_inverse(chn, (void*)dst_ptr, (const void*)src_ptr);
#else
    uintptr_t dst_ptr, src_ptr = (uintptr_t)src, end = src_ptr + len;
    unsigned int length = iio_channel_get_data_format(chn)->length / 8;
    uintptr_t buf_end = (uintptr_t)iio_buffer_end(buf);
    ptrdiff_t buf_step = iio_buffer_step(buf) * (interpolation + 1);

    for (dst_ptr = (uintptr_t)iio_buffer_first(buf, chn);
         dst_ptr < buf_end && src_ptr + length <= end;
         dst_ptr += buf_step, src_ptr += length)
        iio_channel_convert_inverse(chn, (void*)dst_ptr, (const void*)src_ptr);
#endif
}

void device_sink_impl::set_len_tag_key(const std::string& len_tag_key)
{
    if (!len_tag_key.size()) {
        d_len_tag_key = pmt::PMT_NIL;
    } else {
        d_len_tag_key = pmt::string_to_symbol(len_tag_key);
    }
}

int device_sink_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    int ret;

    if (d_len_tag_key != pmt::PMT_NIL) {

        size_t ninputs;
        if (override_tagged_input_channels > 0)
            ninputs = override_tagged_input_channels;
        else
            ninputs = input_items.size();

        for (size_t i = 0; i < ninputs; i++) {
            auto items_read = nitems_read(i);
            get_tags_in_range(d_tags, i, items_read, items_read + 1, d_len_tag_key);

            if (d_tags.size() < 1) {
                throw std::runtime_error("device_sink: Input stream not tagged! Either "
                                         "disable tagged input or tag your stream!");
            }

            long required_size = buffer_size / (interpolation + 1);
            for (auto& tag : d_tags) {
                auto packet_len = pmt::to_long(tag.value);
                if (packet_len != required_size) {
                    throw std::runtime_error("Invalid packet size, expected " +
                                             std::to_string(required_size) + " and got " +
                                             std::to_string(packet_len) + "!");
                }
            }
        }
        // Everything is ok, we can proceed
    }

    if (interpolation >= 1) {
#ifdef LIBIIO_V1
        ptrdiff_t len =
            (intptr_t)iio_block_end(iioblock) - (intptr_t)iio_block_start(iioblock);
        memset(iio_block_start(iioblock), 0, len);
#else
        ptrdiff_t len = (intptr_t)iio_buffer_end(buf) - (intptr_t)iio_buffer_start(buf);
        memset(iio_buffer_start(buf), 0, len);
#endif
    }

    for (unsigned int i = 0; i < input_items.size(); i++)
        channel_write(channel_list[i], input_items[i], noutput_items * sizeof(short));

#ifdef LIBIIO_V1
    if (cyclic) {
        ret = iio_block_enqueue(cyclic_block, 0, true);

        if (ret == 0 && !stream_started) {
            /* The buffer's worker only runs once the stream is started, and a
             * block enqueued before that is never transferred. */
            ret = iio_buffer_stream_start(buf_stream);
            if (ret == 0)
                stream_started = true;
        }

        if (ret == 0)
            ret = iio_block_dequeue(cyclic_block, false);
    } else {
        iioblock = iio_stream_get_next_block(stream);
        ret = -iio_err(iioblock);
    }
#else
    ret = iio_buffer_push(buf);
#endif
    if (ret < 0) {
        char buf[256];
        iio_strerror(-ret, buf, sizeof(buf));
        std::string error(buf);

        d_logger->warn("Unable to push buffer: {:s}", error);
        return WORK_DONE; /* EOF */
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
