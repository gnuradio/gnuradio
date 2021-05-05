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

void bind_device_source(py::module& m);
void bind_attr_source(py::module& m);
void bind_device_sink(py::module& m);
void bind_attr_sink(py::module& m);
void bind_dds_control(py::module& m);
void bind_attr_updater(py::module& m);
#ifdef GR_IIO_LIBAD9361
// void bind_fmcomms2_sink(py::module& m);
// void bind_fmcomms2_sink_f32c(py::module& m);
// void bind_fmcomms5_sink(py::module& m);
// void bind_fmcomms5_sink_f32c(py::module& m);
// void bind_fmcomms2_source(py::module& m);
// void bind_fmcomms2_source_f32c(py::module& m);
// void bind_fmcomms5_source(py::module& m);
// void bind_fmcomms5_source_f32c(py::module& m);
void bind_pluto_sink(py::module& m);
void bind_pluto_source(py::module& m);
#endif

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(iio_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    bind_device_source(m);
    bind_attr_source(m);
    bind_device_sink(m);
    bind_attr_sink(m);
    bind_dds_control(m);
    bind_attr_updater(m);
#ifdef GR_IIO_LIBAD9361
    // bind_fmcomms2_sink(m);
    // bind_fmcomms2_sink_f32c(m);
    // bind_fmcomms5_sink(m);
    // bind_fmcomms5_sink_f32c(m);
    bind_pluto_sink(m);
    // bind_fmcomms2_source(m);
    // bind_fmcomms2_source_f32c(m);
    // bind_fmcomms5_source(m);
    // bind_fmcomms5_source_f32c(m);
    bind_pluto_source(m);
#endif
}
