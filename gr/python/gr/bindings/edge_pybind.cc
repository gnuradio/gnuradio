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

#include <gnuradio/edge.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_edge(py::module& m)
{
    using edge = ::gr::edge;
    using node_endpoint = ::gr::node_endpoint;

    py::class_<node_endpoint, std::shared_ptr<node_endpoint>>(m, "endpoint")
        .def("node", &node_endpoint::node)
        .def("port", &node_endpoint::port);

    py::class_<edge, std::shared_ptr<edge>>(m, "edge")
        .def(py::init(
            [](gr::node_sptr a, gr::port_ptr b, gr::node_sptr c, gr::port_ptr d) {
                return ::gr::edge::make(a, b, c, d);
            }))
        .def("set_custom_buffer", &edge::set_custom_buffer)
        .def("identifier", &edge::identifier)
        .def("src", &edge::src)
        .def("dst", &edge::dst);
}
