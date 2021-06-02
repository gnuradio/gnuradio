/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

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

} // namespace iio
} // namespace gr
