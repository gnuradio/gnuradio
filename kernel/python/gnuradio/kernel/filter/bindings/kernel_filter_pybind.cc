
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

#include <vector>

namespace py = pybind11;

void bind_firdes(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(kernel_filter_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    py::module::import("gnuradio.kernel.fft");

    init_numpy();
    bind_firdes(m);
}
