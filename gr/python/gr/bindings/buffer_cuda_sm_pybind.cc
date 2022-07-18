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

#include <gnuradio/buffer_cuda_sm.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_buffer_cuda_sm(py::module& m)
{
    using buffer_cuda_sm_properties = ::gr::buffer_cuda_sm_properties;

    py::enum_<gr::buffer_cuda_sm_type>(m, "buffer_cuda_sm_type")
        .value("H2D", gr::buffer_cuda_sm_type::H2D)
        .value("D2D", gr::buffer_cuda_sm_type::D2D)
        .value("D2H", gr::buffer_cuda_sm_type::D2H)
        .export_values();


    py::class_<buffer_cuda_sm_properties,
               gr::buffer_properties,
               std::shared_ptr<buffer_cuda_sm_properties>>(m, "buffer_cuda_sm_properties")
        .def_static("make",
                    &buffer_cuda_sm_properties::make,
                    py::arg("buffer_type") = gr::buffer_cuda_sm_type::D2D);
}
