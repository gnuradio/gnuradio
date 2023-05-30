/* -*- c++ -*- */
/*
 * Copyright 2023 Analog Devices Inc.
 * Author: Adrian Suciu <adrian.suciu@analog.com>

 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_IIO_IIO_PRIV_H
#define INCLUDED_IIO_IIO_PRIV_H

#include <iio.h>

namespace gr {
namespace iio {
inline unsigned int channel_size(const iio_channel* chn)
{
    // to round up a / b without moving to float, you can do (a + (b-1)) / b
    // (31 + 7) / 8 = 4 // 31 bits - 4 bytes
    // (32 + 7) / 8 = 4 // 32 bits - 4 bytes
    // (33 + 7) / 8 = 5 // 33 bits - 5 bytes
    const iio_data_format* data = iio_channel_get_data_format(chn);
    return (data->length + (8 - 1)) / 8;
}

} // namespace iio
} // namespace gr

#endif // INCLUDED_IIO_IIO_PRIV_H
