/* -*- c++ -*- */
/*
 * Copyright 2014 Analog Devices Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_DEVICE_SOURCE_IMPL_H
#define INCLUDED_IIO_DEVICE_SOURCE_IMPL_H

#include <gnuradio/iio/device_source.h>
#include <iio.h>

#include <condition_variable>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace gr {
namespace iio {

struct ctxInfo {
    std::string uri;
    struct iio_context* ctx;
    int count;
};

static std::mutex ctx_mutex;
static std::vector<ctxInfo> contexts;

typedef std::vector<ctxInfo>::iterator ctx_it;

class device_source_impl : public device_source
{
private:
    void channel_read(const struct iio_channel* chn, void* dst, size_t len);

    /*std::condition_variable iio_cond, iio_cond2;*/
    std::mutex iio_mutex;
    std::thread refill_thd;

    unsigned long items_in_buffer;
    off_t byte_offset;
    volatile bool please_refill_buffer;
    pmt::pmt_t port_id;

    unsigned long timeout;

    void refill_thread();

protected:
    struct iio_context* ctx;
    struct iio_device *dev, *phy;
    struct iio_buffer* buf;
    std::vector<struct iio_channel*> channel_list;
    unsigned int buffer_size;
    unsigned int decimation;
    bool destroy_ctx;
    volatile bool thread_stopped;


public:
    device_source_impl(struct iio_context* ctx,
                       bool destroy_ctx,
                       const std::string& device,
                       const std::vector<std::string>& channels,
                       const std::string& device_phy,
                       const std::vector<std::string>& params,
                       unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                       unsigned int decimation = 0);
    ~device_source_impl();

    static void set_params(struct iio_device* phy,
                           const std::vector<std::string>& params);

    void set_params(const std::vector<std::string>& params);
    void set_buffer_size(unsigned int buffer_size);
    void set_timeout_ms(unsigned long timeout);

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    bool start();
    bool stop();

    static void remove_ctx_history(struct iio_context* ctx, bool destroy_ctx);

    static struct iio_context* get_context(const std::string& uri);
    static bool load_fir_filter(std::string& filter, struct iio_device* phy);
    static int handle_decimation_interpolation(unsigned long samplerate,
                                        const char* channel_name,
                                        const char* attr_name,
                                        struct iio_device* dev,
                                        bool disable_dec,
                                        bool output_chan);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SOURCE_IMPL_H */
