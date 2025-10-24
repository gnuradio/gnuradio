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

#ifndef INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_IMPL_H
#define INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_IMPL_H

#include <linux_crypto/kernel_crypto_aes.h>
#include <gnuradio/io_signature.h>
#include <vector>
#include <mutex>
#include <string>

namespace gr {
namespace linux_crypto {

class kernel_crypto_aes_impl : public kernel_crypto_aes
{
private:
    std::vector<unsigned char> d_key;
    std::vector<unsigned char> d_iv;
    std::string d_mode;
    bool d_encrypt;
    bool d_kernel_crypto_available;
    std::mutex d_mutex;
    
    int d_socket_fd;
    int d_accept_fd;
    
    void connect_to_kernel_crypto();
    void disconnect_from_kernel_crypto();
    void process_data(const unsigned char* input, unsigned char* output, int n_items);

public:
    kernel_crypto_aes_impl(const std::vector<unsigned char>& key,
                          const std::vector<unsigned char>& iv,
                          const std::string& mode,
                          bool encrypt);
    ~kernel_crypto_aes_impl();

    bool is_kernel_crypto_available() const override;
    std::vector<unsigned char> get_key() const override;
    std::vector<unsigned char> get_iv() const override;
    std::string get_mode() const override;
    bool is_encrypt() const override;
    void set_key(const std::vector<unsigned char>& key) override;
    void set_iv(const std::vector<unsigned char>& iv) override;
    void set_mode(const std::string& mode) override;
    void set_encrypt(bool encrypt) override;
    std::vector<std::string> get_supported_modes() const override;
    std::vector<int> get_supported_key_sizes() const override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace linux_crypto
} // namespace gr

#endif /* INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_IMPL_H */
