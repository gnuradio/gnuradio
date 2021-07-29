/* -*- c++ -*- */
/*
 * Copyright 2021 BlackLynx, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_BUFFER_CONTEXT_H
#define INCLUDED_GR_RUNTIME_BUFFER_CONTEXT_H

#include <gnuradio/api.h>
#include <ostream>

namespace gr {

enum class buffer_context {
    DEFAULT_INVALID,
    HOST_TO_DEVICE,
    DEVICE_TO_HOST,
    HOST_TO_HOST,
    DEVICE_TO_DEVICE
};

GR_RUNTIME_API std::ostream& operator<<(std::ostream& os, const buffer_context& context);
} // namespace gr

#endif
