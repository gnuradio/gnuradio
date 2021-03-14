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

#include "pdu_to_tagged_stream_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/pdu.h>

namespace gr {
namespace pdu {

pdu_to_tagged_stream::sptr pdu_to_tagged_stream::make(gr::types::vector_type type,
                                                      const std::string& tsb_tag_key)
{
    return gnuradio::make_block_sptr<pdu_to_tagged_stream_impl>(type, tsb_tag_key);
}

pdu_to_tagged_stream_impl::pdu_to_tagged_stream_impl(gr::types::vector_type type,
                                                     const std::string& tsb_tag_key)
    : tagged_stream_block("pdu_to_tagged_stream",
                          io_signature::make(0, 0, 0),
                          io_signature::make(1, 1, pdu::itemsize(type)),
                          tsb_tag_key),
      d_itemsize(pdu::itemsize(type)),
      d_curr_len(0)
{
    message_port_register_in(msgport_names::pdus());
}

int pdu_to_tagged_stream_impl::calculate_output_stream_length(const gr_vector_int&)
{
    if (d_curr_len == 0) {
        pmt::pmt_t msg(delete_head_nowait(msgport_names::pdus()));
        if (msg.get() == NULL) {
            return 0;
        }

        if (!pmt::is_pair(msg))
            throw std::runtime_error("received a malformed pdu message");

        d_curr_meta = pmt::car(msg);
        d_curr_vect = pmt::cdr(msg);
        // do not assume the length of  PMT is in items (e.g.: from socket_pdu)
        d_curr_len = pmt::blob_length(d_curr_vect) / d_itemsize;
    }

    return d_curr_len;
}

int pdu_to_tagged_stream_impl::work(int noutput_items,
                                    gr_vector_int& ninput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    uint8_t* out = (uint8_t*)output_items[0];

    if (d_curr_len == 0) {
        return 0;
    }

    // work() should only be called if the current PDU fits entirely
    // into the output buffer.
    assert(noutput_items >= 0 && (unsigned int)noutput_items >= d_curr_len);

    // Copy vector output
    size_t nout = d_curr_len;
    size_t io(0);
    const uint8_t* ptr = (const uint8_t*)uniform_vector_elements(d_curr_vect, io);
    memcpy(out, ptr, d_curr_len * d_itemsize);

    // Copy tags
    if (!pmt::eq(d_curr_meta, pmt::PMT_NIL)) {
        pmt::pmt_t klist(pmt::dict_keys(d_curr_meta));
        for (size_t i = 0; i < pmt::length(klist); i++) {
            pmt::pmt_t k(pmt::nth(i, klist));
            pmt::pmt_t v(pmt::dict_ref(d_curr_meta, k, pmt::PMT_NIL));
            add_item_tag(0, nitems_written(0), k, v, alias_pmt());
        }
    }

    // Reset state
    d_curr_len = 0;

    return nout;
} /* work() */

} /* namespace pdu */
} /* namespace gr */
