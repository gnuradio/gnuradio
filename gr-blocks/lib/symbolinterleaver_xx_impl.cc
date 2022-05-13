/* -*- c++ -*- */
/*
 * Copyright 2022 Johannes Demel.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "symbolinterleaver_xx_impl.h"
#include <gnuradio/io_signature.h>
#include <type_traits>

namespace gr {
namespace blocks {

template <class T>
typename symbolinterleaver_xx<T>::sptr symbolinterleaver_xx<T>::make(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode)
{
    return gnuradio::make_block_sptr<symbolinterleaver_xx_impl<T>>(
        interleaver_indices, is_packed, interleave_mode);
}


/*
 * These are explicit template declarations to make sure that all these specialized make
 * functions are present in the compiled shared library. So far this is unnecessary for
 * other blocks. However, the reasons are yet unknown.
 */
template typename symbolinterleaver_xx<uint8_t>::sptr symbolinterleaver_xx<uint8_t>::make(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode);

template typename symbolinterleaver_xx<gr_complex>::sptr
symbolinterleaver_xx<gr_complex>::make(std::vector<size_t> interleaver_indices,
                                       bool is_packed,
                                       bool interleave_mode);

template typename symbolinterleaver_xx<float>::sptr symbolinterleaver_xx<float>::make(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode);

template typename symbolinterleaver_xx<int32_t>::sptr symbolinterleaver_xx<int32_t>::make(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode);

template typename symbolinterleaver_xx<int16_t>::sptr symbolinterleaver_xx<int16_t>::make(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode);


template <class T>
symbolinterleaver_xx_impl<T>::symbolinterleaver_xx_impl(
    std::vector<size_t> interleaver_indices, bool is_packed, bool interleave_mode)
    : gr::sync_block("symbolinterleaver",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T))),
      d_is_packed(is_packed),
      d_interleave_mode(interleave_mode),
      d_unpacker(std::make_unique<gr::blocks::kernel::unpack_k_bits>(8)),
      d_packer(std::make_unique<gr::blocks::kernel::pack_k_bits>(8)),
      d_interleaver(std::make_unique<symbol_interleaving>(interleaver_indices))
{
    if (not std::is_same<T, uint8_t>::value) {
        is_packed = false;
        d_is_packed = false;
    }
    if (is_packed and d_interleaver->interleaver_length() % 8) {
        throw std::invalid_argument(
            "Packed Interleaver requires 'interleaver_indices' to be a multiple of 8!");
    }
    d_unpacked_original.resize(d_interleaver->interleaver_length());
    d_unpacked_interleaved.resize(d_interleaver->interleaver_length());

    gr::sync_block::set_output_multiple(is_packed
                                            ? (d_interleaver->interleaver_length() / 8)
                                            : d_interleaver->interleaver_length());
}


template <class T>
void symbolinterleaver_xx_impl<T>::interleave_packed(T* out,
                                                     const T* in,
                                                     const unsigned nbytes_per_frame)
{
    throw std::logic_error(
        "Wrong type! packed interleaver is only available for uint8_t!");
}

template <>
void symbolinterleaver_xx_impl<uint8_t>::interleave_packed(
    uint8_t* out, const uint8_t* in, const unsigned nbytes_per_frame)
{
    d_unpacker->unpack(d_unpacked_original.data(), in, nbytes_per_frame);
    d_interleaver->interleave(d_unpacked_interleaved.data(), d_unpacked_original.data());
    d_packer->pack(out, d_unpacked_interleaved.data(), nbytes_per_frame);
}

template <class T>
void symbolinterleaver_xx_impl<T>::deinterleave_packed(T* out,
                                                       const T* in,
                                                       const unsigned nbytes_per_frame)
{
    throw std::logic_error(
        "Wrong type! packed interleaver is only available for uint8_t!");
}

template <>
void symbolinterleaver_xx_impl<uint8_t>::deinterleave_packed(
    uint8_t* out, const uint8_t* in, const unsigned nbytes_per_frame)
{
    d_unpacker->unpack(d_unpacked_original.data(), in, nbytes_per_frame);
    d_interleaver->deinterleave(d_unpacked_interleaved.data(),
                                d_unpacked_original.data());
    d_packer->pack(out, d_unpacked_interleaved.data(), nbytes_per_frame);
}

template <class T>
int symbolinterleaver_xx_impl<T>::work(int noutput_items,
                                       gr_vector_const_void_star& input_items,
                                       gr_vector_void_star& output_items)
{
    const T* in = (const T*)input_items[0];
    T* out = (T*)output_items[0];

    const unsigned nframes =
        (d_is_packed ? 8 : 1) * noutput_items / d_interleaver->interleaver_length();
    const unsigned nbytes_per_frame =
        d_interleaver->interleaver_length() / (d_is_packed ? 8 : 1);

    if (d_interleave_mode) {
        if (d_is_packed) {
            for (unsigned i = 0; i < nframes; ++i) {
                interleave_packed(out, in, nbytes_per_frame);

                in += nbytes_per_frame;
                out += nbytes_per_frame;
            }
        } else {
            for (unsigned i = 0; i < nframes; ++i) {
                d_interleaver->interleave(out, in);
                in += nbytes_per_frame;
                out += nbytes_per_frame;
            }
        }
    } else {
        if (d_is_packed) {
            for (unsigned i = 0; i < nframes; ++i) {
                deinterleave_packed(out, in, nbytes_per_frame);

                in += nbytes_per_frame;
                out += nbytes_per_frame;
            }
        } else {
            for (unsigned i = 0; i < nframes; ++i) {
                d_interleaver->deinterleave(out, in);
                in += nbytes_per_frame;
                out += nbytes_per_frame;
            }
        }
    }

    noutput_items = nframes * nbytes_per_frame;
    return noutput_items;
}

template class symbolinterleaver_xx_impl<uint8_t>;
template class symbolinterleaver_xx_impl<gr_complex>;
template class symbolinterleaver_xx_impl<float>;
template class symbolinterleaver_xx_impl<int32_t>;
template class symbolinterleaver_xx_impl<int16_t>;
} /* namespace blocks */
} /* namespace gr */
