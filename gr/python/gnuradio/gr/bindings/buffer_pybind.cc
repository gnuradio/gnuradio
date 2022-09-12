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

#include <gnuradio/buffer.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_buffer(py::module& m)
{
    using buffer_properties = ::gr::buffer_properties;

    py::class_<buffer_properties, std::shared_ptr<buffer_properties>>(m,
                                                                      "buffer_properties")
        .def("set_buffer_size", &buffer_properties::set_buffer_size)
        .def("set_max_buffer_size", &buffer_properties::set_max_buffer_size)
        .def("set_min_buffer_size", &buffer_properties::set_min_buffer_size)
        .def("set_max_buffer_fill", &buffer_properties::set_max_buffer_fill)
        .def("set_min_buffer_fill", &buffer_properties::set_min_buffer_fill)
        .def("set_max_buffer_read", &buffer_properties::set_max_buffer_read)
        .def("set_min_buffer_read", &buffer_properties::set_min_buffer_read);
}
