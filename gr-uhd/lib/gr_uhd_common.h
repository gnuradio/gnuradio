/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_UHD_COMMON_H
#define INCLUDED_GR_UHD_COMMON_H

#include <uhd/version.hpp>
#include <boost/format.hpp>
#include <stdexcept>

namespace gr {
  namespace uhd {

    static inline void check_abi(void)
    {
#ifdef UHD_VERSION_ABI_STRING
      if(std::string(UHD_VERSION_ABI_STRING) == ::uhd::get_abi_string())
        return;

      throw std::runtime_error(str(boost::format(
        "\nGR-UHD detected ABI compatibility mismatch with UHD library.\n"
        "GR-UHD was build against ABI: %s,\n"
        "but UHD library reports ABI: %s\n"
        "Suggestion: install an ABI compatible version of UHD,\n"
        "or rebuild GR-UHD component against this ABI version.\n"
      ) % UHD_VERSION_ABI_STRING % ::uhd::get_abi_string()));
#endif
    }

  } /* namespace uhd */
} /* namespace gr */

/*!
 * The stream args ensure function sanitizes random user input.
 * We may extend this to handle more things in the future,
 * but ATM it ensures that the channels are initialized.
 */
static inline uhd::stream_args_t stream_args_ensure(const uhd::stream_args_t &args)
{
    uhd::stream_args_t sanitized = args;
    if (sanitized.channels.empty())
    {
        sanitized.channels.push_back(0);
    }
    return sanitized;
}

#endif /* INCLUDED_GR_UHD_COMMON_H */
