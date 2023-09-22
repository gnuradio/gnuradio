/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/transfer_type.h>

namespace gr {

std::ostream& operator<<(std::ostream& os, const transfer_type& type)
{
    switch (type) {
    case transfer_type::DEFAULT_INVALID:
        return os << "DEFAULT_INVALID";
    case transfer_type::HOST_TO_DEVICE:
        return os << "HOST_TO_DEVICE";
    case transfer_type::DEVICE_TO_HOST:
        return os << "DEVICE_TO_HOST";
    case transfer_type::HOST_TO_HOST:
        return os << "HOST_TO_HOST";
    case transfer_type::DEVICE_TO_DEVICE:
        return os << "DEVICE_TO_DEVICE";
    default:
        return os << "Unknown transfer type: " << static_cast<int>(type);
    }
}

} // namespace gr
