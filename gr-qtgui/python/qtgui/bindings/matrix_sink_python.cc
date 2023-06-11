/*
 * Copyright 2023 Free Software Foundation, Inc.
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

#include <gnuradio/qtgui/matrix_sink.h>
// pydoc.h is automatically generated in the build directory
#include <matrix_sink_pydoc.h>

void bind_matrix_sink(py::module& m)
{

    using matrix_sink = gr::qtgui::matrix_sink;


    py::class_<matrix_sink, gr::block, gr::basic_block, std::shared_ptr<matrix_sink>>(
        m, "matrix_sink", D(matrix_sink))

        .def(py::init(&matrix_sink::make),
             py::arg("name"),
             py::arg("num_cols"),
             py::arg("vlen"),
             py::arg("contour"),
             py::arg("color_map"),
             py::arg("interpolation"),
             py::arg("x_start"),
             py::arg("x_end"),
             py::arg("y_start"),
             py::arg("y_end"),
             py::arg("z_max"),
             py::arg("z_min"),
             py::arg("x_axis_label"),
             py::arg("y_axis_label"),
             py::arg("z_axis_label"),
             py::arg("parent") = nullptr,
             D(matrix_sink, make))


        .def("exec_", &matrix_sink::exec_, D(matrix_sink, exec_))


        .def(
            "qwidget",
            [](matrix_sink& self) { return reinterpret_cast<uintptr_t>(self.qwidget()); },
            D(matrix_sink, qwidget))


        ;
}
