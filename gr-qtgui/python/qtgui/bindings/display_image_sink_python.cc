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

#include <gnuradio/qtgui/display_image_sink.h>
// pydoc.h is automatically generated in the build directory
#include <display_image_sink_pydoc.h>

void bind_display_image_sink(py::module& m)
{

    using display_image = ::gr::qtgui::display_image;


    py::class_<display_image,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<display_image>>(m, "display_image", D(display_image))

        .def(py::init(&display_image::make),
             py::arg("imagewidth"),
             py::arg("imageheight"),
             py::arg("parent") = nullptr,
             D(display_image, make))

        .def("exec_", &display_image::exec_, D(display_image, exec_))

        .def(
            "qwidget",
            [](display_image& self) {
                return reinterpret_cast<uintptr_t>(self.qwidget());
            },
            D(display_image, qwidget))

        .def("displayBottomUp",
             &display_image::displayBottomUp,
             py::arg("direction"),
             D(display_image, displayBottomUp))

        ;
}
