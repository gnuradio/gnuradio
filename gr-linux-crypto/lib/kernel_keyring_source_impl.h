/* -*- c++ -*- */
/*
 * Copyright 2024 GNU Radio
 *
 * This file is part of gr-linux-crypto.
 *
 * gr-linux-crypto is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * gr-linux-crypto is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with gr-linux-crypto; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_IMPL_H
#define INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_IMPL_H

#include <linux_crypto/kernel_keyring_source.h>
#include <gnuradio/io_signature.h>
#include <keyutils.h>
#include <vector>
#include <mutex>

namespace gr {
namespace linux_crypto {

class kernel_keyring_source_impl : public kernel_keyring_source
{
private:
    key_serial_t d_key_id;
    bool d_auto_repeat;
    std::vector<unsigned char> d_key_data;
    size_t d_key_size;
    bool d_key_loaded;
    std::mutex d_mutex;

    void load_key_from_keyring();
    void output_key_data(int noutput_items, gr_vector_void_star& output_items);

public:
    kernel_keyring_source_impl(key_serial_t key_id, bool auto_repeat);
    ~kernel_keyring_source_impl();

    bool is_key_loaded() const override;
    size_t get_key_size() const override;
    key_serial_t get_key_id() const override;
    void set_auto_repeat(bool repeat) override;
    bool get_auto_repeat() const override;
    void reload_key() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace linux_crypto
} // namespace gr

#endif /* INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_IMPL_H */
