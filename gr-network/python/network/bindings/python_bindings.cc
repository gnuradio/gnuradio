
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

namespace py = pybind11;

// void bind_packet_headers(py::module&);
void bind_socket_pdu(py::module&);
void bind_tcp_sink(py::module&);
void bind_tuntap_pdu(py::module&);
// void bind_udp_header_types(py::module&);
void bind_udp_sink(py::module&);
void bind_udp_source(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(network_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    // bind_packet_headers(m);
    bind_socket_pdu(m);
    bind_tcp_sink(m);
    bind_tuntap_pdu(m);
    // bind_udp_header_types(m);
    bind_udp_sink(m);
    bind_udp_source(m);
}
