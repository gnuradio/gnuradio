
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

void bind_cfo_model(py::module&);
void bind_channel_model(py::module&);
void bind_channel_model2(py::module&);
void bind_dynamic_channel_model(py::module&);
void bind_fading_model(py::module&);
void bind_selective_fading_model(py::module&);
void bind_selective_fading_model2(py::module&);
void bind_sro_model(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(channels_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_cfo_model(m);
    bind_channel_model(m);
    bind_channel_model2(m);
    bind_dynamic_channel_model(m);
    bind_fading_model(m);
    bind_selective_fading_model(m);
    bind_selective_fading_model2(m);
    bind_sro_model(m);
}
