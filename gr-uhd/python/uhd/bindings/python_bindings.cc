/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <numpy/arrayobject.h>

#include <uhd/device.hpp>
#include <uhd/types/device_addr.hpp>
#include <uhd/types/time_spec.hpp>
#include <uhd/version.hpp>

namespace py = pybind11;

void bind_uhd_types(py::module&);
void bind_amsg_source(py::module&);
void bind_usrp_block(py::module&);
void bind_usrp_sink(py::module&);
void bind_usrp_source(py::module&);
void bind_rfnoc_block(py::module&);
void bind_rfnoc_ddc(py::module&);
void bind_rfnoc_duc(py::module&);
void bind_rfnoc_block_generic(py::module&);
void bind_rfnoc_siggen(py::module&);
void bind_rfnoc_window(py::module&);
void bind_rfnoc_fir_filter(py::module&);
void bind_rfnoc_fft(py::module&);
void bind_rfnoc_graph(py::module&);
void bind_rfnoc_replay(py::module&);
void bind_rfnoc_rx_radio(py::module&);
void bind_rfnoc_rx_streamer(py::module&);
void bind_rfnoc_tx_radio(py::module&);
void bind_rfnoc_tx_streamer(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(uhd_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_uhd_types(m);
    bind_amsg_source(m);
    bind_usrp_block(m);
    bind_usrp_sink(m);
    bind_usrp_source(m);

#ifdef GR_ENABLE_UHD_RFNOC
    bind_rfnoc_block(m);
    bind_rfnoc_ddc(m);
    bind_rfnoc_duc(m);
    bind_rfnoc_block_generic(m);
    bind_rfnoc_siggen(m);
    bind_rfnoc_window(m);
    bind_rfnoc_fir_filter(m);
    bind_rfnoc_fft(m);
    bind_rfnoc_graph(m);
    bind_rfnoc_replay(m);
    bind_rfnoc_rx_radio(m);
    bind_rfnoc_rx_streamer(m);
    bind_rfnoc_tx_radio(m);
    bind_rfnoc_tx_streamer(m);
#endif


    py::implicitly_convertible<double, uhd::time_spec_t>();

    m.def(
        "get_version_string",
        []() { return ::uhd::get_version_string(); },
        "Returns UHD Version String");

    m.def("find", [](const uhd::device_addr_t& hint) { return uhd::device::find(hint); });
}
