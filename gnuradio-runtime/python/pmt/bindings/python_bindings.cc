
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

void bind_pmt(py::module&);
void bind_pmt_pool(py::module&);
// void bind_pmt_serial_tags(py::module&);
void bind_pmt_sugar(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(pmt_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    // py::module::import("gnuradio.gr");

    bind_pmt(m);
    bind_pmt_pool(m);
    // bind_pmt_serial_tags(m);
    bind_pmt_sugar(m);
}
