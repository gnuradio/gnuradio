/*
 * Copyright 2022 Josh Morman
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <gnuradio/pyblock_detail.h>
#include <gnuradio/rpc_client_interface.h>

namespace py = pybind11;

void bind_rpc_client_interface(py::module& m)
{
    py::class_<gr::rpc_client_interface, std::shared_ptr<gr::rpc_client_interface>>(
        m, "rpc_client_interface")
        .def(py::init<>())
        .def("set_pyblock_detail", &gr::rpc_client_interface::set_pyblock_detail);
}
