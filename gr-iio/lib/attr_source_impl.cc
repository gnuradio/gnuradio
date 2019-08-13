/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "attr_source_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/chrono.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>
#include <vector>


namespace gr {
namespace iio {

attr_source::sptr attr_source::make(const std::string& uri,
                                    const std::string& device,
                                    const std::string& channel,
                                    const std::string& attribute,
                                    int update_interval_ms,
                                    int samples_per_update,
                                    int data_type,
                                    int attr_type,
                                    bool output,
                                    uint32_t address,
                                    bool required_enable)
{
    return gnuradio::get_initial_sptr(new attr_source_impl(uri,
                                                           device,
                                                           channel,
                                                           attribute,
                                                           update_interval_ms,
                                                           samples_per_update,
                                                           data_type,
                                                           attr_type,
                                                           output,
                                                           address,
                                                           required_enable));
}

size_t attr_source_impl::type_sizeof(int data_type, int attr_type)
{
    size_t dsize = 0;
    if (attr_type == 3)
        dsize = sizeof(int);
    else {
        switch (data_type) {
        case 0:
            dsize = sizeof(double);
            break;
        case 1:
            dsize = sizeof(float);
            break;
        case 2:
            dsize = sizeof(long long);
            break;
        case 3:
            dsize = sizeof(int);
            break;
        case 4:
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
                                   int data_type,
                                   int attr_type,
                                   bool output,
                                   uint32_t address,
                                   bool required_enable)
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
      output(output),
      address(address),
      required_enable(required_enable)
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
    if (attr_type == 0) {
        chan = iio_device_find_channel(dev, channel.c_str(), output);
        if (!chan) {
            iio_context_destroy(ctx);
            throw std::runtime_error("Channel not found");
        }
    }

    // Required for MathWorks generated IP
    if ((attr_type == 3) && required_enable) {
        int ret = iio_device_attr_write(dev, "reg_access", "enabled");

        if (ret < 0) {
            char error[1024];
            sprintf(error,
                    "Failed to enabled register for device: %s [%d]",
                    device.c_str(),
                    ret);
            throw std::runtime_error(error);
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
        std::cerr << "Reading parameter failed: " << ret << std::endl;
}

void attr_source_impl::get_register_data(uint32_t address, int* value)
{
    uint32_t u32value;
    ret = iio_device_reg_read(dev, address, &u32value);
    attr_source_impl::check(ret);
    *value = boost::lexical_cast<int>(u32value);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, double* value)
{
    switch (attr_type) {
    case 0:
        ret = iio_channel_attr_read_double(chan, attribute.c_str(), value);
        break;
    case 1:
        ret = iio_device_attr_read_double(dev, attribute.c_str(), value);
        break;
    default:
        ret = iio_device_debug_attr_read_double(dev, attribute.c_str(), value);
        break;
    }
    attr_source_impl::check(ret);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, float* value)
{
    double dvalue;
    switch (attr_type) {
    case 0:
        ret = iio_channel_attr_read_double(chan, attribute.c_str(), &dvalue);
        break;
    case 1:
        ret = iio_device_attr_read_double(dev, attribute.c_str(), &dvalue);
        break;
    default:
        ret = iio_device_debug_attr_read_double(dev, attribute.c_str(), &dvalue);
        break;
    }
    attr_source_impl::check(ret);
    *value = boost::lexical_cast<float>(dvalue);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, long long* value)
{
    switch (attr_type) {
    case 0:
        ret = iio_channel_attr_read_longlong(chan, attribute.c_str(), value);
        break;
    case 1:
        ret = iio_device_attr_read_longlong(dev, attribute.c_str(), value);
        break;
    default:
        ret = iio_device_debug_attr_read_longlong(dev, attribute.c_str(), value);
        break;
    }
    attr_source_impl::check(ret);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, int* value)
{
    long long llvalue;
    switch (attr_type) {
    case 0:
        ret = iio_channel_attr_read_longlong(chan, attribute.c_str(), &llvalue);
        break;
    case 1:
        ret = iio_device_attr_read_longlong(dev, attribute.c_str(), &llvalue);
        break;
    default:
        ret = iio_device_debug_attr_read_longlong(dev, attribute.c_str(), &llvalue);
        break;
    }
    attr_source_impl::check(ret);
    *value = boost::lexical_cast<int>(llvalue);
}

void attr_source_impl::get_attribute_data(const std::string& attribute, uint8_t* value)
{
    bool bvalue;
    switch (attr_type) {
    case 0:
        ret = iio_channel_attr_read_bool(chan, attribute.c_str(), &bvalue);
        break;
    case 1:
        ret = iio_device_attr_read_bool(dev, attribute.c_str(), &bvalue);
        break;
    default:
        ret = iio_device_debug_attr_read_bool(dev, attribute.c_str(), &bvalue);
        break;
    }
    attr_source_impl::check(ret);
    *value = boost::lexical_cast<uint8_t>(bvalue);
}

int attr_source_impl::work(int noutput_items,
                           gr_vector_const_void_star& input_items,
                           gr_vector_void_star& output_items)
{
    int sample;
    void* out;

    out = output_items[0];

    for (sample = 0; sample < samples_per_update; sample++) {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(update_interval_ms));
        if (attr_type == 3)
            get_register_data(address, (((int*)out) + sample));
        else {
            switch (data_type) {
            case 0:
                get_attribute_data(attribute, ((double*)out + sample));
                break;
            case 1:
                get_attribute_data(attribute, ((float*)out + sample));
                break;
            case 2:
                get_attribute_data(attribute, ((long long*)out + sample));
                break;
            case 3:
                get_attribute_data(attribute, ((int*)out + sample));
                break;
            case 4:
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
