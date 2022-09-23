/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_BLOCKS_BLOCKINTERLEAVING_H
#define INCLUDED_GR_BLOCKS_BLOCKINTERLEAVING_H

#include <gnuradio/blocks/api.h>
#include <cstddef>
#include <vector>

namespace gr {
namespace blocks {
namespace kernel {

class BLOCKS_API block_interleaving
{
public:
    block_interleaving(std::vector<size_t> interleaver_indices);

    size_t interleaver_length() { return _interleaver_indices.size(); };
    std::vector<size_t> interleaver_indices() { return _interleaver_indices; };
    std::vector<size_t> deinterleaver_indices() { return _deinterleaver_indices; };

    template <class T>
    void interleave(T* target, const T* src)
    {
        for (auto idx : _interleaver_indices) {
            *target++ = src[idx];
        }
    }

    template <class T>
    void deinterleave(T* target, const T* src)
    {
        for (auto idx : _deinterleaver_indices) {
            *target++ = src[idx];
        }
    }

private:
    std::vector<size_t> _interleaver_indices;
    std::vector<size_t> _deinterleaver_indices;

    void set_interleaver_indices(const std::vector<size_t>& interleaver_indices);
};

} /* namespace kernel */
} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_BLOCKS_BLOCKINTERLEAVING_H */
