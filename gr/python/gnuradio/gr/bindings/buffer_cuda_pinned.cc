/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/buffer_cuda_pinned.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_buffer_cuda_pinned(py::module& m)
{
    using buffer_cuda_pinned_properties = ::gr::buffer_cuda_pinned_properties;

    py::class_<buffer_cuda_pinned_properties,
               gr::buffer_properties,
               std::shared_ptr<buffer_cuda_pinned_properties>>(
        m, "buffer_cuda_pinned_properties")
        .def_static("make", &buffer_cuda_pinned_properties::make);
}
