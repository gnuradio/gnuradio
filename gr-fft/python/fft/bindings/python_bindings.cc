
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

#ifdef GR_HAVE_CTRLPORT
void bind_ctrlport_probe_psd(py::module&);
#endif
void bind_fft_shift(py::module&);
void bind_fft_v(py::module&);
void bind_goertzel(py::module&);
void bind_goertzel_fc(py::module&);
void bind_window(py::module&);

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(fft_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

#ifdef GR_HAVE_CTRLPORT
    bind_ctrlport_probe_psd(m);
#endif
    bind_fft_shift(m);
    bind_fft_v(m);
    bind_goertzel(m);
    bind_goertzel_fc(m);
    bind_window(m);
}
