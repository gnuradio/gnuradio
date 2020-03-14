/* -*- c++ -*- */
/*
 * Copyright 2006,2013,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


#ifndef UNPACKED_TO_PACKED_H
#define UNPACKED_TO_PACKED_H

#include <gnuradio/block.h>
#include <gnuradio/blocks/api.h>
#include <gnuradio/endianness.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Convert a stream of unpacked bytes or shorts into a stream of packed bytes or
 * shorts. \ingroup byte_operators_blk
 *
 * \details
 * input: stream of T; output: stream of T
 *
 * This is the inverse of gr::blocks::packed_to_unpacked_XX.
 *
 * The low \p bits_per_chunk bits are extracted from each input
 * byte or short.  These bits are then packed densely into the
 * output bytes or shorts, such that all 8 or 16 bits of the
 * output bytes or shorts are filled with valid input bits.  The
 * right thing is done if bits_per_chunk is not a power of two.
 *
 * The combination of gr::blocks::packed_to_unpacked_XX followed by
 * gr_chunks_to_symbols_Xf or gr_chunks_to_symbols_Xc handles the
 * general case of mapping from a stream of bytes or shorts into
 * arbitrary float or complex symbols.
 *
 * \sa gr::blocks::packed_to_unpacked_bb, gr::blocks::unpacked_to_packed_bb,
 * \sa gr::blocks::packed_to_unpacked_ss, gr::blocks::unpacked_to_packed_ss,
 * \sa gr::blocks::chunks_to_symbols_bf, gr::blocks::chunks_to_symbols_bc.
 * \sa gr::blocks::chunks_to_symbols_sf, gr::blocks::chunks_to_symbols_sc.
 */
template <class T>
class BLOCKS_API unpacked_to_packed : virtual public block
{
public:
    // gr::blocks::unpacked_to_packed::sptr
    typedef std::shared_ptr<unpacked_to_packed<T>> sptr;

    static sptr make(unsigned int bits_per_chunk, endianness_t endianness);
};

typedef unpacked_to_packed<std::uint8_t> unpacked_to_packed_bb;
typedef unpacked_to_packed<std::int16_t> unpacked_to_packed_ss;
typedef unpacked_to_packed<std::int32_t> unpacked_to_packed_ii;
} /* namespace blocks */
} /* namespace gr */

#endif /* UNPACKED_TO_PACKED_H */
