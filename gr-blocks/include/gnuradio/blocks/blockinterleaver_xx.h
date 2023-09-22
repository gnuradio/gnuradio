/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDE_BLOCKINTERLEAVER_H
#define INCLUDE_BLOCKINTERLEAVER_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Fully parameterizable block interleaver
 * \ingroup blocks
 *
 * \details
 * This block accepts item multiples of size \p interleaver_indices.size() and interleaves
 * or deinterleaves them on the output.
 *
 * \param interleaver_indices indices of items in output vector.
 * \param interleave_mode Switch between interleaver [true] and deinterleaver mode
 * [false].
 * \param is_packed Assume packed bytes. For uint8_t I/O only.
 */
template <class T>
class BLOCKS_API blockinterleaver_xx : virtual public sync_block
{
public:
    typedef std::shared_ptr<blockinterleaver_xx<T>> sptr;

    /*!
     * \brief Build a blockinterleaver block
     *
     * \param interleaver_indices indices of items in output vector.
     * \param interleave_mode Switch between interleaver [true] and deinterleaver mode
     * [false].
     * \param is_packed Assume packed bytes.
     */
    static sptr make(std::vector<size_t> interleaver_indices,
                     bool interleave_mode,
                     bool is_packed = false);

    /*!
     * \brief Return interleaver indices
     */
    virtual std::vector<size_t> interleaver_indices() const = 0;

    /*!
     * \brief Return derived de-interleaver indices
     */
    virtual std::vector<size_t> deinterleaver_indices() const = 0;

    /*!
     * \brief Return true if packed bytes are expected
     */
    virtual bool is_packed() const = 0;

    /*!
     * \brief Return true if in interleaver mode, otherwise false
     */
    virtual bool interleaver_mode() const = 0;
};

typedef blockinterleaver_xx<uint8_t> blockinterleaver_bb;
typedef blockinterleaver_xx<gr_complex> blockinterleaver_cc;
typedef blockinterleaver_xx<float> blockinterleaver_ff;
typedef blockinterleaver_xx<int32_t> blockinterleaver_ii;
typedef blockinterleaver_xx<int16_t> blockinterleaver_ss;
} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDE_BLOCKINTERLEAVER_H */
