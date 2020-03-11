/* -*- c++ -*- */
/*
 * Copyright 2015-2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_IMPL_H
#define INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_IMPL_H

#include <gnuradio/digital/protocol_formatter_async.h>

namespace gr {
namespace digital {

class protocol_formatter_async_impl : public protocol_formatter_async
{
private:
    header_format_base::sptr d_format;

    pmt::pmt_t d_in_port;
    pmt::pmt_t d_hdr_port, d_pld_port;

    void append(pmt::pmt_t msg);

public:
    protocol_formatter_async_impl(const header_format_base::sptr& format);
    ~protocol_formatter_async_impl();
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_PROTOCOL_FORMATTER_ASYNC_IMPL_H */
