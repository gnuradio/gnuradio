
/*
 * flowgraphright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <chrono>
#include <thread>

namespace py = pybind11;
using namespace std::chrono_literals;


#include <gnuradio/flowgraph.h>
// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_flowgraph(py::module& m)
{
    py::class_<gr::flat_graph, gr::graph, gr::node, std::shared_ptr<gr::flat_graph>>(
        m, "flat_graph");

    py::class_<gr::flowgraph, gr::graph, gr::node, std::shared_ptr<gr::flowgraph>>(
        m, "flowgraph")
        .def(py::init(&gr::flowgraph::make), py::arg("name") = "flowgraph")
        .def_static("check_connections", &gr::flowgraph::check_connections)
        .def("make_flat", &::gr::flowgraph::make_flat)

        .def("start", &::gr::flowgraph::start, py::call_guard<py::gil_scoped_release>())
        .def("stop", &::gr::flowgraph::stop, py::call_guard<py::gil_scoped_release>())
        .def(
            "wait",
            [](gr::flowgraph::sptr fg) {
                bool ready{ false };
                std::thread th([&fg, &ready] {
                    fg->wait();
                    ready = true;
                });
                th.detach();

                while (!ready) {
                    {
                        py::gil_scoped_acquire acquire;
                        if (PyErr_CheckSignals() != 0) {
                            fg->kill();
                            if (th.joinable()) {
                                th.join();
                            }
                            throw py::error_already_set();
                        }
                    }
                    std::this_thread::sleep_for(100us);
                }
            },
            py::call_guard<py::gil_scoped_release>())
        .def(
            "run",
            [](gr::flowgraph::sptr fg) {
                bool ready{ false };
                std::thread th([&fg, &ready] {
                    fg->run();
                    ready = true;
                });
                th.detach();

                while (!ready) {
                    {
                        py::gil_scoped_acquire acquire;
                        if (PyErr_CheckSignals() != 0) {
                            fg->kill();
                            if (th.joinable()) {
                                th.join();
                            }
                            throw py::error_already_set();
                        }
                    }
                    std::this_thread::sleep_for(100us);
                }
            },
            py::call_guard<py::gil_scoped_release>());
    ;
}
