/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDE_BLOCKINTERLEAVER_IMPL_H
#define INCLUDE_BLOCKINTERLEAVER_IMPL_H

#include <gnuradio/blocks/blockinterleaver_xx.h>
#include <gnuradio/blocks/blockinterleaving.h>
#include <gnuradio/blocks/pack_k_bits.h>
#include <gnuradio/blocks/unpack_k_bits.h>
#include <memory>
#include <vector>

namespace gr {
namespace blocks {

template <class T>
class BLOCKS_API blockinterleaver_xx_impl : public blockinterleaver_xx<T>
{
private:
    bool d_interleave_mode; // True==interleave, False==deinterleave
    bool d_is_packed;

    std::unique_ptr<gr::blocks::kernel::unpack_k_bits> d_unpacker;
    std::unique_ptr<gr::blocks::kernel::pack_k_bits> d_packer;
    std::unique_ptr<gr::blocks::kernel::block_interleaving> d_interleaver;

    std::vector<uint8_t> d_unpacked_original;
    std::vector<uint8_t> d_unpacked_interleaved;

    void interleave_packed(T* out, const T* in, const unsigned nbytes_per_frame);
    void deinterleave_packed(T* out, const T* in, const unsigned nbytes_per_frame);

public:
    blockinterleaver_xx_impl(std::vector<size_t> interleaver_indices,
                             bool interleave_mode,
                             bool is_packed = false);

    std::vector<size_t> interleaver_indices() const override
    {
        return d_interleaver->interleaver_indices();
    }

    std::vector<size_t> deinterleaver_indices() const override
    {
        return d_interleaver->deinterleaver_indices();
    }
    bool is_packed() const override { return d_is_packed; }
    bool interleaver_mode() const override { return d_interleave_mode; }

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDE_BLOCKINTERLEAVER_IMPL_H */
