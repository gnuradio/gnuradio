


/*
 * Copyright 2020 Free Software Foundation, Inc.
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

#include <gnuradio/runtime_proxy.h>

namespace py = pybind11;

void bind_runtime_proxy(py::module& m)
{

    py::class_<gr::runtime_proxy, std::shared_ptr<gr::runtime_proxy>>(m, "runtime_proxy")

        .def(py::init(&gr::runtime_proxy::make))
        .def("svr_port", &gr::runtime_proxy::svr_port)
        .def("client_connect", &gr::runtime_proxy::client_connect);
}
