/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "tagged_stream_to_pdu_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

tagged_stream_to_pdu::sptr tagged_stream_to_pdu::make(gr::types::vector_type type,
                                                      const std::string& lengthtagname)
{
    return gnuradio::make_block_sptr<tagged_stream_to_pdu_impl>(type, lengthtagname);
}

tagged_stream_to_pdu_impl::tagged_stream_to_pdu_impl(gr::types::vector_type type,
                                                     const std::string& lengthtagname)
    : tagged_stream_block("tagged_stream_to_pdu",
                          io_signature::make(1, 1, pdu::itemsize(type)),
                          io_signature::make(0, 0, 0),
                          lengthtagname),
      d_type(type),
      d_pdu_meta(pmt::PMT_NIL),
      d_pdu_vector(pmt::PMT_NIL)
{
    message_port_register_out(msgport_names::pdus());
}

int tagged_stream_to_pdu_impl::work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    const uint8_t* in = (const uint8_t*)input_items[0];

    // Grab tags, throw them into dict
    get_tags_in_range(d_tags, 0, nitems_read(0), nitems_read(0) + ninput_items[0]);
    d_pdu_meta = pmt::make_dict();
    for (const auto& tag : d_tags) {
        d_pdu_meta = dict_add(d_pdu_meta, tag.key, tag.value);
    }

    // Grab data, throw into vector
    d_pdu_vector = pdu::make_pdu_vector(d_type, in, ninput_items[0]);

    // Send msg
    pmt::pmt_t msg = pmt::cons(d_pdu_meta, d_pdu_vector);
    message_port_pub(msgport_names::pdus(), msg);

    return ninput_items[0];
}

} /* namespace pdu */
} /* namespace gr */
