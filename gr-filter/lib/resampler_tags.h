/* -*- c++ -*- */
/*
 * Copyright 2026 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_RESAMPLER_TAGS_H
#define INCLUDED_FILTER_RESAMPLER_TAGS_H

#include <gmpxx.h>
#include <cstdint>

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

} /* namespace filter */
} /* namespace gr */

#endif /* INCLUDED_FILTER_RESAMPLER_TAGS_H */
