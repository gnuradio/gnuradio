/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "attr_source_impl.h"
#include <gnuradio/io_signature.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

namespace gr {
namespace iio {

attr_source::sptr attr_source::make(const std::string& uri,
                                    const std::string& device,
                                    const std::string& channel,
                                    const std::string& attribute,
                                    int update_interval_ms,
                                    int samples_per_update,
                                    data_type_t data_type,
                                    attr_type_t attr_type,
                                    bool output,
                                    uint32_t address)
{
    return gnuradio::make_block_sptr<attr_source_impl>(uri,
                                                       device,
                                                       channel,
                                                       attribute,
                                                       update_interval_ms,
                                                       samples_per_update,
                                                       data_type,
                                                       attr_type,
                                                       output,
                                                       address);
}

size_t attr_source_impl::type_sizeof(data_type_t data_type, attr_type_t attr_type)
{
    size_t dsize = 0;
    if (attr_type == attr_type_t::DEVICE_DEBUG)
        dsize = sizeof(int);
    else {
        switch (data_type) {
        case data_type_t::DOUBLE:
            dsize = sizeof(double);
            break;
        case data_type_t::FLOAT:
            dsize = sizeof(float);
            break;
        case data_type_t::LONGLONG:
            dsize = sizeof(long long);
            break;
        case data_type_t::INT:
            dsize = sizeof(int);
            break;
        case data_type_t::UINT8:
            dsize = sizeof(uint8_t);
            break;
        }
    }
    return dsize;
}

/*
 * The private constructor
 */
attr_source_impl::attr_source_impl(const std::string& uri,
                                   const std::string& device,
                                   const std::string& channel,
                                   const std::string& attribute,
                                   int update_interval_ms,
                                   int samples_per_update,
                                   data_type_t data_type,
                                   attr_type_t attr_type,
                                   bool output,
                                   uint32_t address)
    : gr::sync_block("attr_source",
                     gr::io_signature::make(0, 0, 0),
                     gr::io_signature::make(1, -1, type_sizeof(data_type, attr_type))),
      device(device),
      channel(channel),
      uri(uri),
      attribute(attribute),
      update_interval_ms(update_interval_ms),
      samples_per_update(samples_per_update),
      attr_type(attr_type),
      data_type(data_type),
      address(address)
{
    ctx = device_source_impl::get_context(uri);
    if (!ctx)
        throw std::runtime_error("Unable to create context");

    dev = iio_context_find_device(ctx, device.c_str());
    if (!dev) {
        iio_context_destroy(ctx);
        throw std::runtime_error("Device not found");
    }
    // Channel only needed for channel attributes
    if (attr_type == attr_type_t::CHANNEL) {
        chan = iio_device_find_channel(dev, channel.c_str(), output);
        if (!chan) {
            iio_context_destroy(ctx);
            throw std::runtime_error("Channel not found");
        }
    }

    set_output_multiple(samples_per_update);
}

/*
 * Our virtual destructor.
 */
attr_source_impl::~attr_source_impl() {}

void attr_source_impl::check(int ret)
{
    if (ret < 0)
        d_logger->warn("Reading parameter failed: {:d}", ret);
}

void attr_source_impl::get_register_data(uint32_t address, int* value)
{
    uint32_t u32value;
    ret = iio_device_reg_read(dev, address, &u32value);
    attr_source_impl::check(ret);
    *value = static_cast<int>(u32value);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, double* value)
{
    const struct iio_attr* iio_attribute = nullptr;

    switch (attr_type) {
    case attr_type_t::CHANNEL:
        iio_attribute = iio_channel_find_attr(chan, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    case attr_type_t::DEVICE:
        iio_attribute = iio_device_find_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    default:
        iio_attribute = iio_device_find_debug_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    }
    if (iio_attribute)
        ret = iio_attr_read_double(iio_attribute, value);

    attr_source_impl::check(ret);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, float* value)
{
    double dvalue;
    const struct iio_attr* iio_attribute = nullptr;

    switch (attr_type) {
    case attr_type_t::CHANNEL:
        iio_attribute = iio_channel_find_attr(chan, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    case attr_type_t::DEVICE:
        iio_attribute = iio_device_find_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    default:
        iio_attribute = iio_device_find_debug_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    }
    if (iio_attribute)
        ret = iio_attr_read_double(iio_attribute, &dvalue);
    attr_source_impl::check(ret);
    *value = static_cast<float>(dvalue);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, long long* value)
{
    const struct iio_attr* iio_attribute = nullptr;

    switch (attr_type) {
    case attr_type_t::CHANNEL:
        iio_attribute = iio_channel_find_attr(chan, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    case attr_type_t::DEVICE:
        iio_attribute = iio_device_find_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    default:
        iio_attribute = iio_device_find_debug_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    }
    if (iio_attribute)
        ret = iio_attr_read_longlong(iio_attribute, value);
    attr_source_impl::check(ret);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, int* value)
{
    long long llvalue;
    const struct iio_attr* iio_attribute = nullptr;

    switch (attr_type) {
    case attr_type_t::CHANNEL:
        iio_attribute = iio_channel_find_attr(chan, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    case attr_type_t::DEVICE:
        iio_attribute = iio_device_find_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    default:
        iio_attribute = iio_device_find_debug_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    }
    if (iio_attribute)
        ret = iio_attr_read_longlong(iio_attribute, &llvalue);
    attr_source_impl::check(ret);
    *value = static_cast<int>(llvalue);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, uint8_t* value)
{
    bool bvalue;
    const struct iio_attr* iio_attribute = nullptr;

    switch (attr_type) {
    case attr_type_t::CHANNEL:
        iio_attribute = iio_channel_find_attr(chan, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    case attr_type_t::DEVICE:
        iio_attribute = iio_device_find_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    default:
        iio_attribute = iio_device_find_debug_attr(dev, attribute.c_str());
        if (!iio_attribute)
            ret = -EINVAL;
        break;
    }
    if (iio_attribute)
        ret = iio_attr_read_bool(iio_attribute, &bvalue);
    attr_source_impl::check(ret);
    *value = static_cast<uint8_t>(bvalue);
}

int attr_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    int sample;
    void* out;

    out = output_items[0];

    for (sample = 0; sample < samples_per_update; sample++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(update_interval_ms));
        if (attr_type == attr_type_t::DEVICE_DEBUG)
            get_register_data(address, (((int*)out) + sample));
        else {
            switch (data_type) {
            case data_type_t::DOUBLE:
                get_attribute_data(attribute, ((double*)out + sample));
                break;
            case data_type_t::FLOAT:
                get_attribute_data(attribute, ((float*)out + sample));
                break;
            case data_type_t::LONGLONG:
                get_attribute_data(attribute, ((long long*)out + sample));
                break;
            case data_type_t::INT:
                get_attribute_data(attribute, ((int*)out + sample));
                break;
            case data_type_t::UINT8:
                get_attribute_data(attribute, ((uint8_t*)out + sample));
                break;
            }
        }
    }

    // Tell runtime system how many output items we produced.
    return samples_per_update;
}

} /* namespace iio */
} /* namespace gr */
