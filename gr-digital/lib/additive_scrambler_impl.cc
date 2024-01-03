/* -*- c++ -*- */
/*
 * Copyright 2008,2010,2012 Free Software Foundation, Inc.
 * Copyright 2024 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "additive_scrambler_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace digital {

template <class T>
typename additive_scrambler<T>::sptr
additive_scrambler<T>::make(uint64_t mask,
                            uint64_t seed,
                            uint8_t len,
                            int64_t count,
                            uint8_t bits_per_byte,
                            const std::string& reset_tag_key)
{
    return gnuradio::make_block_sptr<additive_scrambler_impl<T>>(
        mask, seed, len, count, bits_per_byte, reset_tag_key);
}

template <class T>
additive_scrambler_impl<T>::additive_scrambler_impl(uint64_t mask,
                                                    uint64_t seed,
                                                    uint8_t len,
                                                    int64_t count,
                                                    uint8_t bits_per_byte,
                                                    const std::string& reset_tag_key)
    : sync_block("additive_scrambler",
                 io_signature::make(1, 1, sizeof(T)),
                 io_signature::make(1, 1, sizeof(T))),
      d_lfsr(mask, seed, len),
      d_count(reset_tag_key.empty() ? count : -1),
      d_bytes(0),
      d_len(len),
      d_seed(seed),
      d_bits_per_byte(bits_per_byte),
      d_reset_tag_key(pmt::string_to_symbol(reset_tag_key))
{
    if (d_count < -1) {
        throw std::invalid_argument("count must be non-negative!");
    }

    if constexpr (is_hard_symbol) {
        if (d_bits_per_byte < 1 || d_bits_per_byte > 8) {
            throw std::invalid_argument("bits_per_byte must be in [1, 8]");
        }
    } else {
        if (d_bits_per_byte != 1) {
            throw std::invalid_argument("bits_per_byte must be 1 in soft-symbol mode");
        }
    }
}

template <class T>
additive_scrambler_impl<T>::~additive_scrambler_impl()
{
}

template <class T>
uint64_t additive_scrambler_impl<T>::mask() const
{
    return d_lfsr.mask();
}

template <class T>
uint64_t additive_scrambler_impl<T>::seed() const
{
    return d_seed;
}

template <class T>
uint8_t additive_scrambler_impl<T>::len() const
{
    return d_len;
}

template <class T>
int64_t additive_scrambler_impl<T>::count() const
{
    return d_count;
}

template <class T>
int64_t
additive_scrambler_impl<T>::_get_next_reset_index(int64_t noutput_items,
                                                  int64_t last_reset_index /* = -1 */)
{
    int64_t reset_index =
        noutput_items; // This is a value that gets never reached in the for loop
    if (d_count == -1) {
        std::vector<gr::tag_t> tags;
        this->get_tags_in_range(tags,
                                0,
                                this->nitems_read(0),
                                this->nitems_read(0) + noutput_items,
                                d_reset_tag_key);
        for (unsigned i = 0; i < tags.size(); i++) {
            int64_t reset_pos = tags[i].offset - this->nitems_read(0);
            if (reset_pos < reset_index && reset_pos > last_reset_index) {
                reset_index = reset_pos;
            }
        }
    } else {
        if (last_reset_index == -1) {
            reset_index = d_count - d_bytes;
        } else {
            reset_index = last_reset_index + d_count;
        }
    }
    return reset_index;
}

template <class T>
int additive_scrambler_impl<T>::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const T* in = static_cast<const T*>(input_items[0]);
    T* out = static_cast<T*>(output_items[0]);
    int reset_index = _get_next_reset_index(noutput_items);

    for (int i = 0; i < noutput_items; i++) {
        // Reset should occur at/before the item associated with the tag.
        if (i == reset_index) {
            d_lfsr.reset();
            d_bytes = 0;
            reset_index = _get_next_reset_index(noutput_items, reset_index);
        }
        if constexpr (is_hard_symbol) {
            unsigned char scramble_byte = 0x00;
            for (int k = 0; k < d_bits_per_byte; k++) {
                scramble_byte ^= (d_lfsr.next_bit() << k);
            }
            out[i] = in[i] ^ scramble_byte;
        } else {
            out[i] = d_lfsr.next_bit() ? -in[i] : in[i];
        }
        d_bytes++;
    }

    return noutput_items;
}

template class additive_scrambler<gr_complex>;
template class additive_scrambler<float>;
template class additive_scrambler<int32_t>;
template class additive_scrambler<int16_t>;
template class additive_scrambler<uint8_t>;
;

} /* namespace digital */
} /* namespace gr */
