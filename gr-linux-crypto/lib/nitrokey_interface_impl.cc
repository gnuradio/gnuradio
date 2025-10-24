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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <linux_crypto/nitrokey_interface_impl.h>
#include <cstring>
#include <stdexcept>

// Note: This is a simplified implementation
// Real implementation would use libnitrokey headers
// #include <libnitrokey/NitrokeyManager.h>

namespace gr {
namespace linux_crypto {

nitrokey_interface::sptr
nitrokey_interface::make(int slot, bool auto_repeat)
{
    return gnuradio::get_initial_sptr(
        new nitrokey_interface_impl(slot, auto_repeat));
}

nitrokey_interface_impl::nitrokey_interface_impl(int slot, bool auto_repeat)
    : gr::block("nitrokey_interface",
                gr::io_signature::make(0, 0, 0),
                gr::io_signature::make(1, 1, sizeof(unsigned char))),
      d_slot(slot),
      d_auto_repeat(auto_repeat),
      d_key_size(0),
      d_key_loaded(false),
      d_nitrokey_available(false),
      d_device(nullptr)
{
    connect_to_nitrokey();
    if (d_nitrokey_available) {
        load_key_from_nitrokey();
    }
}

nitrokey_interface_impl::~nitrokey_interface_impl()
{
    // Clear key data from memory
    if (!d_key_data.empty()) {
        memset(d_key_data.data(), 0, d_key_data.size());
    }
    
    // Disconnect from Nitrokey
    if (d_device) {
        // Real implementation would call libnitrokey disconnect
        d_device = nullptr;
    }
}

void
nitrokey_interface_impl::connect_to_nitrokey()
{
    std::lock_guard<std::mutex> lock(d_mutex);
    
    // Simplified implementation - real version would use libnitrokey
    // This is a placeholder that simulates Nitrokey connection
    
    // In real implementation:
    // 1. Initialize libnitrokey
    // 2. Connect to Nitrokey device
    // 3. Get device information
    // 4. Check if slot is available
    
    // For now, simulate connection
    d_nitrokey_available = true;  // Would be determined by actual libnitrokey call
    d_device_info = "Nitrokey Pro (simulated)";
    
    // Real implementation would be:
    // NK_device* device = NK_connect();
    // if (device) {
    //     d_device = device;
    //     d_nitrokey_available = true;
    //     d_device_info = NK_get_device_info(device);
    // }
}

void
nitrokey_interface_impl::load_key_from_nitrokey()
{
    std::lock_guard<std::mutex> lock(d_mutex);
    
    if (!d_nitrokey_available || !d_device) {
        d_key_loaded = false;
        d_key_size = 0;
        return;
    }
    
    // Simplified implementation - real version would use libnitrokey
    // This is a placeholder that simulates key loading
    
    // In real implementation:
    // 1. Check if slot exists and has data
    // 2. Read key data from slot
    // 3. Verify key integrity
    
    // For now, simulate key loading
    d_key_size = 32;  // Simulate 256-bit key
    d_key_data.resize(d_key_size);
    
    // Generate some test data (in real implementation, this would come from Nitrokey)
    for (size_t i = 0; i < d_key_size; i++) {
        d_key_data[i] = static_cast<unsigned char>((i * 7 + d_slot * 13) % 256);
    }
    
    d_key_loaded = true;
    
    // Real implementation would be:
    // size_t key_size = NK_get_slot_data_size(d_device, d_slot);
    // if (key_size > 0) {
    //     d_key_data.resize(key_size);
    //     if (NK_read_slot_data(d_device, d_slot, d_key_data.data(), key_size)) {
    //         d_key_size = key_size;
    //         d_key_loaded = true;
    //     }
    // }
}

bool
nitrokey_interface_impl::is_nitrokey_available() const
{
    std::lock_guard<std::mutex> lock(d_mutex);
    return d_nitrokey_available;
}

bool
nitrokey_interface_impl::is_key_loaded() const
{
    std::lock_guard<std::mutex> lock(d_mutex);
    return d_key_loaded;
}

size_t
nitrokey_interface_impl::get_key_size() const
{
    std::lock_guard<std::mutex> lock(d_mutex);
    return d_key_size;
}

int
nitrokey_interface_impl::get_slot() const
{
    return d_slot;
}

void
nitrokey_interface_impl::set_auto_repeat(bool repeat)
{
    d_auto_repeat = repeat;
}

bool
nitrokey_interface_impl::get_auto_repeat() const
{
    return d_auto_repeat;
}

void
nitrokey_interface_impl::reload_key()
{
    load_key_from_nitrokey();
}

std::string
nitrokey_interface_impl::get_device_info() const
{
    std::lock_guard<std::mutex> lock(d_mutex);
    return d_device_info;
}

std::vector<int>
nitrokey_interface_impl::get_available_slots() const
{
    std::lock_guard<std::mutex> lock(d_mutex);
    
    // Simplified implementation - real version would query Nitrokey
    std::vector<int> slots;
    
    if (d_nitrokey_available) {
        // Simulate 16 available slots (0-15)
        for (int i = 0; i < 16; i++) {
            slots.push_back(i);
        }
    }
    
    // Real implementation would be:
    // return NK_get_available_slots(d_device);
    
    return slots;
}

int
nitrokey_interface_impl::work(int noutput_items,
                              gr_vector_const_void_star& input_items,
                              gr_vector_void_star& output_items)
{
    unsigned char* out = (unsigned char*)output_items[0];
    
    if (!d_nitrokey_available || !d_key_loaded || d_key_data.empty()) {
        // No Nitrokey or key loaded, output zeros
        memset(out, 0, noutput_items);
        return noutput_items;
    }
    
    if (d_auto_repeat) {
        // Repeat key data to fill output
        for (int i = 0; i < noutput_items; i++) {
            out[i] = d_key_data[i % d_key_data.size()];
        }
    } else {
        // Output key data once, then zeros
        size_t output_size = std::min(static_cast<size_t>(noutput_items), d_key_data.size());
        memcpy(out, d_key_data.data(), output_size);
        
        if (noutput_items > static_cast<int>(output_size)) {
            memset(out + output_size, 0, noutput_items - output_size);
        }
    }
    
    return noutput_items;
}

} // namespace linux_crypto
} // namespace gr
