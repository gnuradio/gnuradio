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

#ifndef INCLUDED_GR_LINUX_CRYPTO_NITROKEY_INTERFACE_H
#define INCLUDED_GR_LINUX_CRYPTO_NITROKEY_INTERFACE_H

#include <gnuradio/block.h>
#include <gnuradio/linux_crypto/api.h>
#include <vector>
#include <string>

namespace gr {
namespace linux_crypto {

/*!
 * \brief Nitrokey hardware security module interface
 * \ingroup linux_crypto
 *
 * This block provides access to Nitrokey hardware security modules
 * for secure key storage and cryptographic operations. This is
 * unique to Linux hardware security and not available in gr-openssl
 * or gr-nacl.
 *
 * The block uses libnitrokey to communicate with Nitrokey devices
 * and provides secure key storage that is protected by hardware.
 *
 * This is designed to work with existing crypto modules:
 * - Use with gr-openssl for OpenSSL operations with hardware keys
 * - Use with gr-nacl for modern crypto operations with hardware keys
 */
class LINUX_CRYPTO_API nitrokey_interface : virtual public gr::block
{
public:
    typedef std::shared_ptr<nitrokey_interface> sptr;

    /*!
     * \brief Make a Nitrokey interface block
     *
     * \param slot The slot number on the Nitrokey device (0-15)
     * \param auto_repeat Whether to auto-repeat the key data
     * \return shared pointer to the new block
     */
    static sptr make(int slot = 0, bool auto_repeat = false);

    /*!
     * \brief Check if Nitrokey is connected and accessible
     * \return true if Nitrokey is available
     */
    virtual bool is_nitrokey_available() const = 0;

    /*!
     * \brief Check if key is loaded from hardware
     * \return true if key is loaded and available
     */
    virtual bool is_key_loaded() const = 0;

    /*!
     * \brief Get the size of the loaded key
     * \return size of the key in bytes
     */
    virtual size_t get_key_size() const = 0;

    /*!
     * \brief Get the slot number
     * \return the slot number on the Nitrokey device
     */
    virtual int get_slot() const = 0;

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
     * \brief Reload key from Nitrokey
     *
     * This method re-reads the key from the Nitrokey device.
     * Useful when the key has been updated on the device.
     */
    virtual void reload_key() = 0;

    /*!
     * \brief Get device information
     * \return string with device information
     */
    virtual std::string get_device_info() const = 0;

    /*!
     * \brief Get available slots
     * \return vector of available slot numbers
     */
    virtual std::vector<int> get_available_slots() const = 0;
};

} // namespace linux_crypto
} // namespace gr

#endif /* INCLUDED_GR_LINUX_CRYPTO_NITROKEY_INTERFACE_H */
