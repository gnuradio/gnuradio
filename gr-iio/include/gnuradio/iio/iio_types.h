/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef _INCLUDED_IIO_TYPES_H
#define _INCLUDED_IIO_TYPES_H

#include <gnuradio/iio/api.h>
#include <charconv>
#include <string>
#include <variant>
#include <vector>

namespace gr {
namespace iio {

enum class data_type_t { DOUBLE = 0, FLOAT = 1, LONGLONG = 2, INT = 3, UINT8 = 4 };

enum class attr_type_t {
    CHANNEL = 0,
    DEVICE = 1,
    DEVICE_BUFFER = 2,
    DEVICE_DEBUG = 3,
    DIRECT_REGISTER_ACCESS = 4
};

typedef std::variant<long long unsigned int, long unsigned int, int, double, std::string>
    iio_param_value_t;

class IIO_API iio_param_t : public std::pair<std::string, std::string>
{
public:
    iio_param_t(const std::string& key, iio_param_value_t value);
    iio_param_t(const std::string& kvpair);
    ~iio_param_t() {}

    static std::string to_string(iio_param_value_t value);
};

typedef std::vector<iio_param_t> iio_param_vec_t;

} // namespace iio
} // namespace gr

#endif
