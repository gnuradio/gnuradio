
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

#include <gnuradio/graph.h>
// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_graph(py::module& m)
{
    py::class_<gr::graph, gr::node, std::shared_ptr<gr::graph>>(m, "graph")
        .def("connect",
             py::overload_cast<std::shared_ptr<gr::node>,
                               unsigned int,
                               std::shared_ptr<gr::node>,
                               unsigned int>(&gr::graph::connect))
        .def("connect",
             py::overload_cast<std::shared_ptr<gr::node>, std::shared_ptr<gr::node>>(
                 &gr::graph::connect))
        .def("connect",
             py::overload_cast<std::shared_ptr<gr::node>,
                               const std::string&,
                               std::shared_ptr<gr::node>,
                               const std::string&>(&gr::graph::connect))
        .def("connect",
             py::overload_cast<std::pair<std::shared_ptr<gr::node>, unsigned int>,
                               std::pair<std::shared_ptr<gr::node>, unsigned int>>(
                 &gr::graph::connect))
        .def("connect",
             py::overload_cast<std::pair<std::shared_ptr<gr::node>, const std::string&>,
                               std::pair<std::shared_ptr<gr::node>, const std::string&>>(
                 &gr::graph::connect))
        .def("connect",
             py::overload_cast<
                 const std::vector<std::pair<std::shared_ptr<gr::node>, unsigned int>>&>(
                 &gr::graph::connect))
        .def("connect",
             py::overload_cast<const std::vector<std::shared_ptr<gr::node>>&>(
                 &gr::graph::connect))
        .def("add_edge", &gr::graph::add_edge)
        .def("edges", &gr::graph::edges);
}
