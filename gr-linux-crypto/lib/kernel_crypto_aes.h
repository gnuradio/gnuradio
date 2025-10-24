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

#ifndef INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_H
#define INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_H

#include <gnuradio/sync_block.h>
#include <gnuradio/linux_crypto/api.h>
#include <string>

namespace gr {
namespace linux_crypto {

/*!
 * \brief Linux kernel crypto API AES encryption/decryption
 * \ingroup linux_crypto
 *
 * This block provides AES encryption/decryption using the Linux kernel
 * crypto API via AF_ALG sockets. This provides hardware acceleration
 * and bypasses user-space crypto libraries when possible.
 *
 * This is unique to Linux and not available in gr-openssl or gr-nacl.
 * It uses the kernel's built-in crypto subsystem for better performance
 * and hardware acceleration.
 *
 * The block uses AF_ALG sockets to communicate with the kernel crypto
 * subsystem, providing direct access to hardware crypto instructions
 * and kernel-optimized implementations.
 */
class LINUX_CRYPTO_API kernel_crypto_aes : virtual public gr::sync_block
{
public:
    typedef std::shared_ptr<kernel_crypto_aes> sptr;

    /*!
     * \brief Make a kernel crypto AES block
     *
     * \param key The AES key (16, 24, or 32 bytes for AES-128, AES-192, AES-256)
     * \param iv The initialization vector (16 bytes for AES)
     * \param mode The AES mode ("cbc", "ecb", "ctr", "gcm")
     * \param encrypt Whether to encrypt (true) or decrypt (false)
     * \return shared pointer to the new block
     */
    static sptr make(const std::vector<unsigned char>& key,
                     const std::vector<unsigned char>& iv,
                     const std::string& mode = "cbc",
                     bool encrypt = true);

    /*!
     * \brief Check if kernel crypto is available
     * \return true if kernel crypto API is available
     */
    virtual bool is_kernel_crypto_available() const = 0;

    /*!
     * \brief Get the current key
     * \return the current AES key
     */
    virtual std::vector<unsigned char> get_key() const = 0;

    /*!
     * \brief Get the current IV
     * \return the current initialization vector
     */
    virtual std::vector<unsigned char> get_iv() const = 0;

    /*!
     * \brief Get the current mode
     * \return the current AES mode
     */
    virtual std::string get_mode() const = 0;

    /*!
     * \brief Check if in encrypt mode
     * \return true if encrypting, false if decrypting
     */
    virtual bool is_encrypt() const = 0;

    /*!
     * \brief Set new key
     * \param key the new AES key
     */
    virtual void set_key(const std::vector<unsigned char>& key) = 0;

    /*!
     * \brief Set new IV
     * \param iv the new initialization vector
     */
    virtual void set_iv(const std::vector<unsigned char>& iv) = 0;

    /*!
     * \brief Set new mode
     * \param mode the new AES mode
     */
    virtual void set_mode(const std::string& mode) = 0;

    /*!
     * \brief Set encrypt/decrypt mode
     * \param encrypt true for encrypt, false for decrypt
     */
    virtual void set_encrypt(bool encrypt) = 0;

    /*!
     * \brief Get supported modes
     * \return vector of supported AES modes
     */
    virtual std::vector<std::string> get_supported_modes() const = 0;

    /*!
     * \brief Get supported key sizes
     * \return vector of supported key sizes in bytes
     */
    virtual std::vector<int> get_supported_key_sizes() const = 0;
};

} // namespace linux_crypto
} // namespace gr

#endif /* INCLUDED_GR_LINUX_CRYPTO_KERNEL_CRYPTO_AES_H */
