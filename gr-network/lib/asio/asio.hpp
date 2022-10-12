/* -*- c++ -*- */
/*
 * Copyright 2022 Martin Braun <martin.braun@ettus.com>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// This is a translation layer from boost::asio to non-Boost asio. It makes
// boost::asio look like regular asio to allow easier transition.

#ifndef INCLUDED_GR_ASIO_WRAPPER_H
#define INCLUDED_GR_ASIO_WRAPPER_H

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>

namespace boost {
namespace asio {
using error_code = boost::system::error_code;
using system_error = boost::system::system_error;
} // namespace asio
} // namespace boost

namespace asio = boost::asio;

#endif /* INCLUDED_GR_ASIO_WRAPPER_H */
