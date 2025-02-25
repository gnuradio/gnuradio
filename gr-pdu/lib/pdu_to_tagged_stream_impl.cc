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
    : sync_block("pdu_to_tagged_stream",
                 io_signature::make(0, 0, 0),
                 io_signature::make(1, 1, pdu::itemsize(type))),
      d_itemsize(pdu::itemsize(type)),
      d_tag(pmt::mp(tsb_tag_key))
{
    message_port_register_in(msgport_names::pdus());
}

int pdu_to_tagged_stream_impl::work(int noutput_items,
                                    gr_vector_const_void_star& input_items,
                                    gr_vector_void_star& output_items)
{
    uint8_t* out = (uint8_t*)output_items[0];
    size_t nout = 0;

    if (d_remain.size() > 0) {
        size_t ncopy = std::min(d_remain.size() / d_itemsize, (size_t)noutput_items);
        memcpy(out, &d_remain[0], ncopy * d_itemsize);
        d_remain.erase(d_remain.begin(), d_remain.begin() + (ncopy * d_itemsize));
        nout += ncopy;
    }

    while (nout < (size_t)noutput_items) {
        pmt::pmt_t msg(delete_head_nowait(msgport_names::pdus()));
        if (msg.get() == nullptr) {
            break;
        }

        if (!pmt::is_pair(msg)) {
            throw std::runtime_error("received a malformed pdu message");
        }

        pmt::pmt_t meta = pmt::car(msg);
        pmt::pmt_t vect = pmt::cdr(msg);
        size_t vect_items = pmt::blob_length(vect) / d_itemsize;

        uint64_t offset = nitems_written(0) + nout;
        add_item_tag(0, offset, d_tag, pmt::from_long(vect_items), alias_pmt());

        // Copy vector output
        size_t ncopy = std::min((size_t)noutput_items - nout, vect_items);
        size_t nsave = vect_items - ncopy;

        size_t io(0);
        const uint8_t* ptr = (const uint8_t*)uniform_vector_elements(vect, io);
        memcpy(out + (nout * d_itemsize), ptr, ncopy * d_itemsize);
        nout += ncopy;

        if (nsave > 0) {
            d_remain.resize(nsave * d_itemsize, 0);
            memcpy(&d_remain[0], ptr + (ncopy * d_itemsize), nsave * d_itemsize);
        }

        // Copy tags
        if (!pmt::eq(meta, pmt::PMT_NIL)) {
            pmt::pmt_t klist(pmt::dict_keys(meta));
            for (size_t i = 0; i < pmt::length(klist); i++) {
                pmt::pmt_t k(pmt::nth(i, klist));
                pmt::pmt_t v(pmt::dict_ref(meta, k, pmt::PMT_NIL));
                add_item_tag(0, offset, k, v, alias_pmt());
            }
        }
    }

    return nout;
} /* work() */

} /* namespace pdu */
} /* namespace gr */
