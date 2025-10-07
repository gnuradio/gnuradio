/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PDU_TO_TAGGED_STREAM_IMPL_H
#define INCLUDED_PDU_TO_TAGGED_STREAM_IMPL_H

#include <gnuradio/pdu/pdu_to_tagged_stream.h>

namespace gr {
namespace pdu {

class PDU_API pdu_to_tagged_stream_impl : public pdu_to_tagged_stream
{
    const size_t d_itemsize;
    std::vector<uint8_t> d_remain;
    pmt::pmt_t d_tag;

public:
    pdu_to_tagged_stream_impl(gr::types::vector_type type,
                              const std::string& lengthtagname = "packet_len");

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace pdu */
} /* namespace gr */

#endif /* INCLUDED_PDU_TO_TAGGED_STREAM_IMPL_H */
