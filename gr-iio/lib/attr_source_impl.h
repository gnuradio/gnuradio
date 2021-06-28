/* -*- c++ -*- */
/*
 * Copyright 2018 Analog Devices Inc.
 * Author: Travis Collins <travis.collins@analog.com>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_ATTR_SOURCE_IMPL_H
#define INCLUDED_IIO_ATTR_SOURCE_IMPL_H

#include "device_source_impl.h"
#include <gnuradio/iio/attr_source.h>

#include <string>

namespace gr {
namespace iio {

class attr_source_impl : public attr_source
{
private:
    std::string device;
    std::string channel;
    std::string uri;
    std::string attribute;
    int update_interval_ms;
    int samples_per_update;
    attr_type_t attr_type;
    data_type_t data_type;
    bool output;
    int ret;
    uint32_t address;

protected:
    struct iio_context* ctx;
    struct iio_device* dev;
    struct iio_channel* chan;

public:
    attr_source_impl(const std::string& uri,
                     const std::string& device,
                     const std::string& channel,
                     const std::string& attribute,
                     int update_interval_ms,
                     int samples_per_update,
                     data_type_t data_type,
                     attr_type_t attr_type,
                     bool output,
                     uint32_t address);
    ~attr_source_impl();

    // Where all the action really happens
    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);

    size_t type_sizeof(data_type_t data_type, attr_type_t attr_type);
    void get_attribute_data(const std::string& attribute, double* value);
    void get_attribute_data(const std::string& attribute, float* value);
    void get_attribute_data(const std::string& attribute, long long* value);
    void get_attribute_data(const std::string& attribute, int* value);
    void get_attribute_data(const std::string& attribute, uint8_t* value);
    void get_register_data(uint32_t address, int* value);
    void check(int ret);
};

} // namespace iio
} // namespace gr

#endif /* INCLUDED_IIO_ATTR_SOURCE_IMPL_H */
