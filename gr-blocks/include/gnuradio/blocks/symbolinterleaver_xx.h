/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDE_SYMBOLINTERLEAVER_H
#define INCLUDE_SYMBOLINTERLEAVER_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/sync_block.h>
#include <cstdint>

namespace gr {
namespace blocks {

/*!
 * \brief Fully parameterizable symbol interleaver
 * \ingroup blocks
 *
 * \details
 * This block accepts item multiples of size \p interleaver_indices.size() and interleaves
 * or deinterleaves them on the output.
 * \param interleaver_indices indices of items in output vector.
 * \param is_packed Assume packed bytes.
 * \param interleave_mode Switch between interleaver [true] and deinterleaver mode [false].
 */
template <class T>
class BLOCKS_API symbolinterleaver_xx : virtual public sync_block
{
public:
    typedef std::shared_ptr<symbolinterleaver_xx<T>> sptr;

    /*!
     * \brief Build a symbolinterleaver block
     *
     * \param interleaver_indices indices of items in output vector.
     * \param is_packed Assume packed bytes.
     * \param interleave_mode Switch between interleaver [true] and deinterleaver mode [false].
     */
    static sptr
    make(std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode);

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

typedef symbolinterleaver_xx<uint8_t> symbolinterleaver_bb;
typedef symbolinterleaver_xx<float> symbolinterleaver_ff;
} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDE_SYMBOLINTERLEAVER_H */
