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

#include "kernel_keyring_source.h"
#include <gnuradio/io_signature.h>
#include <keyutils.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>

namespace gr {
namespace linux_crypto {

class kernel_keyring_source_impl : public kernel_keyring_source
{
private:
    key_serial_t d_key_id;
    std::vector<uint8_t> d_key_data;
    size_t d_key_size;
    size_t d_output_index;
    bool d_auto_repeat;
    bool d_key_loaded;

public:
    kernel_keyring_source_impl(key_serial_t key_id, bool auto_repeat)
        : gr::block("kernel_keyring_source",
                    gr::io_signature::make(0, 0, 0),
                    gr::io_signature::make(1, 1, sizeof(uint8_t))),
          d_key_id(key_id),
          d_output_index(0),
          d_auto_repeat(auto_repeat),
          d_key_loaded(false)
    {
        load_key_from_keyring();
    }

    ~kernel_keyring_source_impl() = default;

    void load_key_from_keyring()
    {
        // Read key from kernel keyring
        long key_len = keyctl_read_alloc(d_key_id, (void**)&d_key_data.data());
        if (key_len < 0) {
            std::cerr << "Failed to read key from keyring: " << strerror(errno)
                      << std::endl;
            d_key_loaded = false;
            return;
        }

        d_key_size = static_cast<size_t>(key_len);
        d_key_data.resize(d_key_size);

        // Re-read with proper buffer
        key_len = keyctl_read(d_key_id, d_key_data.data(), d_key_size);
        if (key_len < 0) {
            std::cerr << "Failed to read key data: " << strerror(errno)
                      << std::endl;
            d_key_loaded = false;
            return;
        }

        d_key_loaded = true;
        d_output_index = 0;
    }

    int general_work(int noutput_items,
                    gr_vector_int& ninput_items,
                    gr_vector_const_void_star& input_items,
                    gr_vector_void_star& output_items) override
    {
        if (!d_key_loaded) {
            return 0; // No data available
        }

        uint8_t* out = static_cast<uint8_t*>(output_items[0]);
        int produced = 0;

        for (int i = 0; i < noutput_items; i++) {
            if (d_output_index >= d_key_size) {
                if (d_auto_repeat) {
                    d_output_index = 0; // Reset to beginning
                } else {
                    break; // No more data
                }
            }

            out[i] = d_key_data[d_output_index];
            d_output_index++;
            produced++;
        }

        return produced;
    }

    // Public interface methods
    bool is_key_loaded() const override { return d_key_loaded; }
    size_t get_key_size() const override { return d_key_size; }
    key_serial_t get_key_id() const override { return d_key_id; }

    void set_auto_repeat(bool repeat) override { d_auto_repeat = repeat; }
    bool get_auto_repeat() const override { return d_auto_repeat; }

    // Reload key from keyring
    void reload_key() override { load_key_from_keyring(); }
};

kernel_keyring_source::sptr kernel_keyring_source::make(key_serial_t key_id,
                                                        bool auto_repeat)
{
    return gnuradio::get_initial_sptr(
        new kernel_keyring_source_impl(key_id, auto_repeat));
}

} // namespace linux_crypto
} // namespace gr