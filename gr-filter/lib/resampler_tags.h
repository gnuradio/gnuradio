/* -*- c++ -*- */
/*
 * Copyright 2026 Anirudh Sahu
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_RESAMPLER_TAGS_H
#define INCLUDED_FILTER_RESAMPLER_TAGS_H

#include <gnuradio/block.h>
#include <gnuradio/block_detail.h>
#ifdef GR_MPLIB_MPIR
#include <mpirxx.h>
#else
#include <gmpxx.h>
#endif
#include <algorithm>
#include <cstdint>
#include <vector>

namespace gr {
namespace filter {

/*!
 * Output offset of a tag on input offset \p in, computed like the default
 * TPP_ALL_TO_ALL propagation in block_executor (input offset times the
 * relative rate, rounded), but counted from the input and output offsets
 * \p in0, \p out0 at which the rate was last set.
 */
inline uint64_t
resampler_tag_offset(uint64_t in, uint64_t in0, uint64_t out0, const mpq_class& rate)
{
    static const mpq_class one_half(1, 2);
    const uint64_t rel = in - in0;
    mpz_class offset;
    mpz_import(offset.get_mpz_t(), 1, 1, sizeof(rel), 0, 0, &rel);
    offset = offset * rate + one_half;
    return out0 + offset.get_ui();
}

/*!
 * Place the tags of input 0 on output 0 of a resampler, called from
 * general_work() before consume/produce. Tags are placed like the default
 * propagation does, but also as soon as the output a tag maps to has been
 * produced: a resampler can produce that output before it consumes the tagged
 * sample, and a tag added only on consumption could land on output that was
 * already read downstream. \p tag_next is the first input offset whose tags
 * are not placed yet.
 */
inline void place_resampler_tags(block& blk,
                                 uint64_t& tag_next,
                                 int ninput,
                                 int consumed,
                                 int produced,
                                 uint64_t in0 = 0,
                                 uint64_t out0 = 0)
{
    const block_detail_sptr d = blk.detail();
    const uint64_t consumed_end = d->nitems_read(0) + consumed;
    const uint64_t written_end = d->nitems_written(0) + produced;
    std::vector<tag_t> tags;
    d->get_tags_in_range(tags, 0, tag_next, d->nitems_read(0) + ninput);
    for (auto& tag : tags) {
        const uint64_t in_offset = tag.offset;
        tag.offset = resampler_tag_offset(in_offset, in0, out0, blk.mp_relative_rate());
        if (in_offset >= consumed_end && tag.offset >= written_end) {
            break;
        }
        d->add_item_tag(0, tag);
        tag_next = in_offset + 1;
    }
    tag_next = std::max(tag_next, consumed_end);
}

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_RESAMPLER_TAGS_H */
