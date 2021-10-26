/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "pack_k_bits_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace blocks {

pack_k_bits_bb::sptr pack_k_bits_bb::make(unsigned k)
{
    return gnuradio::make_block_sptr<pack_k_bits_bb_impl>(k);
}

pack_k_bits_bb_impl::pack_k_bits_bb_impl(unsigned k)
    : sync_decimator("pack_k_bits_bb",
                     io_signature::make(1, 1, sizeof(unsigned char)),
                     io_signature::make(1, 1, sizeof(unsigned char)),
                     k),
      d_pack(k)
{
    d_k = k;
    set_tag_propagation_policy(TPP_CUSTOM);
}

int pack_k_bits_bb_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    std::vector<tag_t> wintags; // Temp variable to store tags for prop

    d_pack.pack(out, in, noutput_items);

    // Propagate tags
    get_tags_in_range(wintags, 0, nitems_read(0), nitems_read(0) + (noutput_items * d_k));

    std::vector<tag_t>::iterator t;
    for (t = wintags.begin(); t != wintags.end(); t++) {
        tag_t new_tag = *t;
        new_tag.offset = std::floor((double)new_tag.offset / d_k);
        add_item_tag(0, new_tag);
    }

    return noutput_items;
}

} /* namespace blocks */
} /* namespace gr */
