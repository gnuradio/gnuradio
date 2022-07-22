
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

#include <gnuradio/hier_block.h>
// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_hier_block(py::module& m)
{
    py::class_<gr::hier_block, gr::block, gr::node, std::shared_ptr<gr::hier_block>>(
        m, "hier_block")
        .def(py::init<const std::string&>())
        .def("connect",
             py::overload_cast<std::shared_ptr<gr::node>,
                               unsigned int,
                               std::shared_ptr<gr::node>,
                               unsigned int>(&gr::hier_block::connect))

        ;
}
