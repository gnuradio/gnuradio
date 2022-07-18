
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

#include <gnuradio/graph_utils.h>
// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_graph_utils(py::module& m)
{
    py::class_<gr::graph_partition_info, std::shared_ptr<gr::graph_partition_info>>(
        m, "graph_partition_info")
        .def_readwrite("scheduler", &gr::graph_partition_info::scheduler)
        .def_readwrite("subgraph", &gr::graph_partition_info::subgraph);

    py::class_<gr::graph_utils, std::shared_ptr<gr::graph_utils>>(m, "graph_utils")
        .def_static("partition", &gr::graph_utils::partition);
}
