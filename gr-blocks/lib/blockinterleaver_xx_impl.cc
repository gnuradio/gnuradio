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

#include "blockinterleaver_xx_impl.h"
#include <gnuradio/io_signature.h>
#include <type_traits>

namespace gr {
namespace blocks {

template <class T>
typename blockinterleaver_xx<T>::sptr blockinterleaver_xx<T>::make(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed)
{
    return gnuradio::make_block_sptr<blockinterleaver_xx_impl<T>>(
        interleaver_indices, interleave_mode, is_packed);
}


/*
 * These are explicit template declarations to make sure that all these specialized make
 * functions are present in the compiled shared library. So far this is unnecessary for
 * other blocks. However, the reasons are yet unknown.
 */
template typename blockinterleaver_xx<uint8_t>::sptr blockinterleaver_xx<uint8_t>::make(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed);

template typename blockinterleaver_xx<gr_complex>::sptr
blockinterleaver_xx<gr_complex>::make(std::vector<size_t> interleaver_indices,
                                      bool interleave_mode,
                                      bool is_packed);

template typename blockinterleaver_xx<float>::sptr blockinterleaver_xx<float>::make(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed);

template typename blockinterleaver_xx<int32_t>::sptr blockinterleaver_xx<int32_t>::make(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed);

template typename blockinterleaver_xx<int16_t>::sptr blockinterleaver_xx<int16_t>::make(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed);


template <class T>
blockinterleaver_xx_impl<T>::blockinterleaver_xx_impl(
    std::vector<size_t> interleaver_indices, bool interleave_mode, bool is_packed)
    : gr::sync_block("blockinterleaver",
                     gr::io_signature::make(1, 1, sizeof(T)),
                     gr::io_signature::make(1, 1, sizeof(T))),
      d_interleave_mode(interleave_mode),
      d_is_packed(is_packed),
      d_unpacker(std::make_unique<gr::blocks::kernel::unpack_k_bits>(8)),
      d_packer(std::make_unique<gr::blocks::kernel::pack_k_bits>(8)),
      d_interleaver(
          std::make_unique<gr::blocks::kernel::block_interleaving>(interleaver_indices))
{
    if (!std::is_same<T, uint8_t>::value) {
        is_packed = false;
        d_is_packed = false;
    }
    if (is_packed && d_interleaver->interleaver_length() % 8) {
        throw std::invalid_argument(fmt::format(
            "Interleaver with packed byte input requires the length of "
            "interleaver_indices to be divisible by 8. {} is not divisible by 8!",
            interleaver_indices.size()));
    }
    d_unpacked_original.resize(d_interleaver->interleaver_length());
    d_unpacked_interleaved.resize(d_interleaver->interleaver_length());

    gr::sync_block::set_output_multiple(is_packed
                                            ? (d_interleaver->interleaver_length() / 8)
                                            : d_interleaver->interleaver_length());
}


template <class T>
void blockinterleaver_xx_impl<T>::interleave_packed(T* out,
                                                    const T* in,
                                                    const unsigned nbytes_per_frame)
{
    throw std::logic_error(
        "Wrong type! packed interleaver is only available for uint8_t!");
}

template <>
void blockinterleaver_xx_impl<uint8_t>::interleave_packed(uint8_t* out,
                                                          const uint8_t* in,
                                                          const unsigned nbytes_per_frame)
{
    d_unpacker->unpack(d_unpacked_original.data(), in, nbytes_per_frame);
    d_interleaver->interleave(d_unpacked_interleaved.data(), d_unpacked_original.data());
    d_packer->pack(out, d_unpacked_interleaved.data(), nbytes_per_frame);
}

template <class T>
void blockinterleaver_xx_impl<T>::deinterleave_packed(T* out,
                                                      const T* in,
                                                      const unsigned nbytes_per_frame)
{
    throw std::logic_error(
        "Wrong type! packed interleaver is only available for uint8_t!");
}

template <>
void blockinterleaver_xx_impl<uint8_t>::deinterleave_packed(
    uint8_t* out, const uint8_t* in, const unsigned nbytes_per_frame)
{
    d_unpacker->unpack(d_unpacked_original.data(), in, nbytes_per_frame);
    d_interleaver->deinterleave(d_unpacked_interleaved.data(),
                                d_unpacked_original.data());
    d_packer->pack(out, d_unpacked_interleaved.data(), nbytes_per_frame);
}

template <class T>
int blockinterleaver_xx_impl<T>::work(int noutput_items,
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

template class blockinterleaver_xx_impl<uint8_t>;
template class blockinterleaver_xx_impl<gr_complex>;
template class blockinterleaver_xx_impl<float>;
template class blockinterleaver_xx_impl<int32_t>;
template class blockinterleaver_xx_impl<int16_t>;
} /* namespace blocks */
} /* namespace gr */
