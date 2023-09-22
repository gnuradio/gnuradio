
/*
 * Copyright 2021 Free Software Foundation, Inc.
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

void bind_add_system_time(py::module&);
void bind_pdu_filter(py::module&);
void bind_pdu_remove(py::module&);
void bind_pdu_set(py::module&);
void bind_pdu_split(py::module&);
void bind_pdu_to_stream(py::module&);
void bind_pdu_to_tagged_stream(py::module&);
void bind_random_pdu(py::module&);
void bind_tags_to_pdu(py::module&);
void bind_tagged_stream_to_pdu(py::module&);
void bind_take_skip_to_pdu(py::module&);
void bind_time_delta(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(pdu_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_add_system_time(m);
    bind_pdu_filter(m);
    bind_pdu_remove(m);
    bind_pdu_set(m);
    bind_pdu_split(m);
    bind_pdu_to_stream(m);
    bind_pdu_to_tagged_stream(m);
    bind_random_pdu(m);
    bind_tags_to_pdu(m);
    bind_tagged_stream_to_pdu(m);
    bind_take_skip_to_pdu(m);
    bind_time_delta(m);
}
