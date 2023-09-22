
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

void bind_pub_msg_sink(py::module&);
void bind_pub_sink(py::module&);
void bind_pull_msg_source(py::module&);
void bind_pull_source(py::module&);
void bind_push_msg_sink(py::module&);
void bind_push_sink(py::module&);
void bind_rep_msg_sink(py::module&);
void bind_rep_sink(py::module&);
void bind_req_msg_source(py::module&);
void bind_req_source(py::module&);
void bind_sub_msg_source(py::module&);
void bind_sub_source(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(zeromq_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_pub_msg_sink(m);
    bind_pub_sink(m);
    bind_pull_msg_source(m);
    bind_pull_source(m);
    bind_push_msg_sink(m);
    bind_push_sink(m);
    bind_rep_msg_sink(m);
    bind_rep_sink(m);
    bind_req_msg_source(m);
    bind_req_source(m);
    bind_sub_msg_source(m);
    bind_sub_source(m);
}
