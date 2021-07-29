/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/buffer_context.h>

namespace gr {

std::ostream& operator<<(std::ostream& os, const buffer_context& context)
{
    switch (context) {
    case buffer_context::DEFAULT_INVALID:
        return os << "DEFAULT_INVALID";
    case buffer_context::HOST_TO_DEVICE:
        return os << "HOST_TO_DEVICE";
    case buffer_context::DEVICE_TO_HOST:
        return os << "DEVICE_TO_HOST";
    case buffer_context::HOST_TO_HOST:
        return os << "HOST_TO_HOST";
    case buffer_context::DEVICE_TO_DEVICE:
        return os << "DEVICE_TO_DEVICE";
    default:
        return os << "Unknown buffer context: " << static_cast<int>(context);
    }
}

} // namespace gr
