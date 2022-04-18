/*
 * Copyright 2022 Free Software Foundation, Inc.
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

#include <gnuradio/qtgui/display_text_sink.h>
// pydoc.h is automatically generated in the build directory
#include <display_text_sink_pydoc.h>

void bind_display_text_sink(py::module& m)
{

    using display_text_sink = ::gr::qtgui::display_text_sink;


    py::class_<display_text_sink,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<display_text_sink>>(
        m, "display_text_sink", D(display_text_sink))

        .def(py::init(&display_text_sink::make),
             py::arg("label"),
             py::arg("splitlength") = 80,
             py::arg("maxline") = 100,
             py::arg("parent") = nullptr,
             D(display_text_sink, make))


        .def(
            "qwidget",
            [](display_text_sink& self) {
                return reinterpret_cast<uintptr_t>(self.qwidget());
            },
            D(display_text_sink, qwidget))


        .def("exec_", &display_text_sink::exec_, D(display_text_sink, exec_))

        ;
}
