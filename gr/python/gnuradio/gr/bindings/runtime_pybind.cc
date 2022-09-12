


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

#include <gnuradio/runtime.h>

#include <chrono>
#include <thread>

namespace py = pybind11;
using namespace std::chrono_literals;

void bind_runtime(py::module& m)
{

    py::class_<gr::runtime, std::shared_ptr<gr::runtime>>(m, "runtime")

        .def(py::init(&gr::runtime::make))
        .def("add_scheduler",
             py::overload_cast<std::pair<gr::scheduler_sptr, std::vector<gr::node_sptr>>>(
                 &gr::runtime::add_scheduler),
             py::arg("conf"))
        .def("add_scheduler",
             py::overload_cast<gr::scheduler_sptr>(&gr::runtime::add_scheduler),
             py::arg("sched"))
        .def("add_proxy", &gr::runtime::add_proxy)
        // .def("clear_schedulers", &gr::flowgraph::clear_schedulers)
        .def("initialize", &gr::runtime::initialize)
        .def("start", &gr::runtime::start, py::call_guard<py::gil_scoped_release>())
        .def("stop", &gr::runtime::stop, py::call_guard<py::gil_scoped_release>())
        .def(
            "wait",
            [](gr::runtime::sptr rt) {
                bool ready{ false };
                std::thread th([&rt, &ready] {
                    rt->wait();
                    ready = true;
                });
                th.detach();

                while (!ready) {
                    {
                        py::gil_scoped_acquire acquire;
                        if (PyErr_CheckSignals() != 0) {
                            rt->kill();
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
            [](gr::runtime::sptr rt) {
                bool ready{ false };
                std::thread th([&rt, &ready] {
                    rt->run();
                    ready = true;
                });
                th.detach();

                while (!ready) {
                    {
                        py::gil_scoped_acquire acquire;
                        if (PyErr_CheckSignals() != 0) {
                            rt->kill();
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
