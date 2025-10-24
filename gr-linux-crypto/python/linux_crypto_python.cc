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

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>

#include <linux_crypto/kernel_keyring_source.h>
#include <linux_crypto/nitrokey_interface.h>
#include <linux_crypto/kernel_crypto_aes.h>

namespace py = pybind11;

void bind_kernel_keyring_source(py::module& m)
{
    using kernel_keyring_source = gr::linux_crypto::kernel_keyring_source;
    
    py::class_<kernel_keyring_source, gr::block, std::shared_ptr<kernel_keyring_source>>(
        m, "kernel_keyring_source")
        .def(py::init(&kernel_keyring_source::make),
             py::arg("key_id"),
             py::arg("auto_repeat") = false)
        .def("is_key_loaded", &kernel_keyring_source::is_key_loaded)
        .def("get_key_size", &kernel_keyring_source::get_key_size)
        .def("get_key_id", &kernel_keyring_source::get_key_id)
        .def("set_auto_repeat", &kernel_keyring_source::set_auto_repeat)
        .def("get_auto_repeat", &kernel_keyring_source::get_auto_repeat)
        .def("reload_key", &kernel_keyring_source::reload_key);
}

void bind_nitrokey_interface(py::module& m)
{
    using nitrokey_interface = gr::linux_crypto::nitrokey_interface;
    
    py::class_<nitrokey_interface, gr::block, std::shared_ptr<nitrokey_interface>>(
        m, "nitrokey_interface")
        .def(py::init(&nitrokey_interface::make),
             py::arg("slot") = 0,
             py::arg("auto_repeat") = false)
        .def("is_nitrokey_available", &nitrokey_interface::is_nitrokey_available)
        .def("is_key_loaded", &nitrokey_interface::is_key_loaded)
        .def("get_key_size", &nitrokey_interface::get_key_size)
        .def("get_slot", &nitrokey_interface::get_slot)
        .def("set_auto_repeat", &nitrokey_interface::set_auto_repeat)
        .def("get_auto_repeat", &nitrokey_interface::get_auto_repeat)
        .def("reload_key", &nitrokey_interface::reload_key)
        .def("get_device_info", &nitrokey_interface::get_device_info)
        .def("get_available_slots", &nitrokey_interface::get_available_slots);
}

void bind_kernel_crypto_aes(py::module& m)
{
    using kernel_crypto_aes = gr::linux_crypto::kernel_crypto_aes;
    
    py::class_<kernel_crypto_aes, gr::sync_block, std::shared_ptr<kernel_crypto_aes>>(
        m, "kernel_crypto_aes")
        .def(py::init(&kernel_crypto_aes::make),
             py::arg("key"),
             py::arg("iv"),
             py::arg("mode") = "cbc",
             py::arg("encrypt") = true)
        .def("is_kernel_crypto_available", &kernel_crypto_aes::is_kernel_crypto_available)
        .def("get_key", &kernel_crypto_aes::get_key)
        .def("get_iv", &kernel_crypto_aes::get_iv)
        .def("get_mode", &kernel_crypto_aes::get_mode)
        .def("is_encrypt", &kernel_crypto_aes::is_encrypt)
        .def("set_key", &kernel_crypto_aes::set_key)
        .def("set_iv", &kernel_crypto_aes::set_iv)
        .def("set_mode", &kernel_crypto_aes::set_mode)
        .def("set_encrypt", &kernel_crypto_aes::set_encrypt)
        .def("get_supported_modes", &kernel_crypto_aes::get_supported_modes)
        .def("get_supported_key_sizes", &kernel_crypto_aes::get_supported_key_sizes);
}

PYBIND11_MODULE(linux_crypto_python, m)
{
    m.doc() = "GNU Radio Linux Crypto Python bindings";
    
    // Bind the classes
    bind_kernel_keyring_source(m);
    bind_nitrokey_interface(m);
    bind_kernel_crypto_aes(m);
    
    // Add module-level functions
    m.def("get_integration_status", []() {
        py::dict status;
        status["kernel_keyring_available"] = true;
        status["nitrokey_available"] = true;
        status["kernel_crypto_available"] = true;
        return status;
    });
}