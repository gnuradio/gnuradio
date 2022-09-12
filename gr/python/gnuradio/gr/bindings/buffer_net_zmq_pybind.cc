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

#include <gnuradio/buffer_net_zmq.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_buffer_net_zmq(py::module& m)
{
    using buffer_net_zmq_properties = ::gr::buffer_net_zmq_properties;

    py::class_<buffer_net_zmq_properties,
               gr::buffer_properties,
               std::shared_ptr<buffer_net_zmq_properties>>(m, "buffer_net_zmq_properties")
        // .def(py::init(
        //     [](const std::string& ipaddr, int port) {
        //         return ::gr::buffer_net_zmq_properties::make(ipaddr, port);
        //     }), py::arg("ipaddr"), py::arg("port"))
        .def_static(
            "make", &buffer_net_zmq_properties::make, py::arg("ipaddr"), py::arg("port"))
        .def_static("make_from_params",
                    &buffer_net_zmq_properties::make_from_params,
                    py::arg("json_str"))
        .def("to_json", &buffer_net_zmq_properties::to_json);
}
