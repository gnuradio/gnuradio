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

#include <gnuradio/qtgui/display_text_msg.h>
// pydoc.h is automatically generated in the build directory
#include <display_text_msg_pydoc.h>

void bind_display_text_msg(py::module& m)
{

    using display_text_msg = ::gr::qtgui::display_text_msg;


    py::class_<display_text_msg,
               gr::block,
               gr::basic_block,
               std::shared_ptr<display_text_msg>>(
        m, "display_text_msg", D(display_text_msg))

        .def(py::init(&display_text_msg::make),
             py::arg("label"),
             py::arg("message_key"),
             py::arg("splitlength") = 80,
             py::arg("maxline") = 100,
             py::arg("parent") = nullptr,
             D(display_text_msg, make))


        .def("exec_", &display_text_msg::exec_, D(display_text_msg, exec_))


        .def(
            "qwidget",
            [](display_text_msg& self) {
                return reinterpret_cast<uintptr_t>(self.qwidget());
            },
            D(display_text_msg, qwidget))

        ;
}
