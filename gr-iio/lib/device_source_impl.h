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

#ifdef LIBIIO_V1
#include <iio/iio.h>
#else
#include <iio.h>
#endif

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace gr {
namespace iio {

struct ctxInfo {
    std::string uri;
    iio_context* ctx;
    int count;
};

static std::mutex ctx_mutex;
static std::vector<ctxInfo> contexts;

typedef std::vector<ctxInfo>::iterator ctx_it;

class device_source_impl : public device_source
{
private:
    void channel_read(const iio_channel* chn, void* dst, size_t len);

    /*std::condition_variable iio_cond, iio_cond2;*/
    std::mutex iio_mutex;
    std::thread refill_thd;

    unsigned long items_in_buffer;
    off_t byte_offset;
    volatile bool please_refill_buffer;
    pmt::pmt_t port_id;

    unsigned long timeout;

    pmt::pmt_t d_len_tag_key;

    void refill_thread();

#ifdef LIBIIO_V1
    static int device_identify_filename(const struct iio_device* dev,
                                        const char* filename,
                                        struct iio_channel** chn,
                                        const struct iio_attr** attr);
#endif

protected:
    iio_context* ctx;
    iio_device *dev, *phy;
    iio_buffer* buf;
#ifdef LIBIIO_V1
    iio_stream* stream;
    const iio_block* iioblock;
    iio_channels_mask* mask;
#endif
    std::vector<iio_channel*> channel_list;
    unsigned int buffer_size;
    unsigned int decimation;
    bool destroy_ctx;
    volatile bool thread_stopped;
    uint16_t override_tagged_output_channels = 0;


public:
    device_source_impl(iio_context* ctx,
                       bool destroy_ctx,
                       const std::string& device,
                       const std::vector<std::string>& channels,
                       const std::string& device_phy,
                       const iio_param_vec_t& params,
                       unsigned int buffer_size = DEFAULT_BUFFER_SIZE,
                       unsigned int decimation = 0);

    ~device_source_impl();

    static void set_params(iio_device* phy, const iio_param_vec_t& params);

    void set_len_tag_key(const std::string& len_tag_key) override;

    void set_params(const iio_param_vec_t& params);
    void set_buffer_size(unsigned int buffer_size) override;
    void set_timeout_ms(unsigned long timeout) override;

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;

    bool start() override;
    bool stop() override;

    static void remove_ctx_history(iio_context* ctx, bool destroy_ctx);

    static iio_context* get_context(const std::string& uri);
    static bool load_fir_filter(std::string& filter, iio_device* phy);
    static int handle_decimation_interpolation(unsigned long samplerate,
                                               const char* channel_name,
                                               const char* attr_name,
                                               iio_device* dev,
                                               bool disable_dec,
                                               bool output_chan);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_DEVICE_SOURCE_IMPL_H */
