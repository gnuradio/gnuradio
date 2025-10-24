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

#ifndef INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_H
#define INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_H

#include <gnuradio/block.h>
#include <gnuradio/linux_crypto/api.h>
#include <keyutils.h>
#include <vector>

namespace gr {
namespace linux_crypto {

/*!
 * \brief Kernel keyring source block
 * \ingroup linux_crypto
 *
 * This block reads keys from the Linux kernel keyring and outputs them
 * as a stream of bytes. This is unique to Linux and not available in
 * gr-openssl or gr-nacl.
 *
 * The block uses the Linux keyctl system calls to access keys stored
 * in the kernel keyring. This provides secure key storage that is
 * protected by the kernel and not accessible to user space processes
 * without proper permissions.
 *
 * This is designed to work with existing crypto modules:
 * - Use with gr-openssl for OpenSSL operations
 * - Use with gr-nacl for modern crypto operations
 */
class LINUX_CRYPTO_API kernel_keyring_source : virtual public gr::block
{
public:
    typedef std::shared_ptr<kernel_keyring_source> sptr;

    /*!
     * \brief Make a kernel keyring source block
     *
     * \param key_id The key ID from the kernel keyring
     * \param auto_repeat Whether to auto-repeat the key data
     * \return shared pointer to the new block
     */
    static sptr make(key_serial_t key_id, bool auto_repeat = false);

    /*!
     * \brief Check if key is loaded
     * \return true if key is loaded and available
     */
    virtual bool is_key_loaded() const = 0;

    /*!
     * \brief Get the size of the loaded key
     * \return size of the key in bytes
     */
    virtual size_t get_key_size() const = 0;

    /*!
     * \brief Get the key ID
     * \return the key ID from the kernel keyring
     */
    virtual key_serial_t get_key_id() const = 0;

    /*!
     * \brief Set auto-repeat mode
     * \param repeat whether to auto-repeat the key data
     */
    virtual void set_auto_repeat(bool repeat) = 0;

    /*!
     * \brief Get auto-repeat mode
     * \return true if auto-repeat is enabled
     */
    virtual bool get_auto_repeat() const = 0;

    /*!
     * \brief Reload key from keyring
     *
     * This method re-reads the key from the kernel keyring.
     * Useful when the key has been updated in the keyring.
     */
    virtual void reload_key() = 0;
};

} // namespace linux_crypto
} // namespace gr

#endif /* INCLUDED_GR_LINUX_CRYPTO_KERNEL_KEYRING_SOURCE_H */