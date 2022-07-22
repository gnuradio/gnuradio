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

#include <gnuradio/block.h>
#include <gnuradio/pyblock_detail.h>

// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_block(py::module& m)
{
    using block = ::gr::block;

    py::class_<block, gr::node, std::shared_ptr<block>>(m, "block")
        .def("work", &block::work, py::arg("work_io"))
        .def("base", &block::base)
        .def_static("cast", &block::cast)
        .def("set_pyblock_detail", &block::set_pyblock_detail)
        .def("request_parameter_query",
             py::overload_cast<int>(&block::request_parameter_query))
        .def("request_parameter_query",
             py::overload_cast<const std::string&>(&block::request_parameter_query))
        .def("request_parameter_change",
             py::overload_cast<int, pmtf::pmt, bool>(&block::request_parameter_change))
        .def("request_parameter_change",
             py::overload_cast<const std::string&, pmtf::pmt, bool>(
                 &block::request_parameter_change))
        .def_static("deserialize_param_to_pmt", &block::deserialize_param_to_pmt)
        .def("to_json", &block::to_json);
}
