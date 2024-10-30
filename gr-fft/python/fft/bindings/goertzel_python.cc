/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(goertzel.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(07fc2dce9bb6928e349c2b0ef5c3ef3f)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fft/goertzel.h>
// pydoc.h is automatically generated in the build directory
#include <goertzel_pydoc.h>

void bind_goertzel(py::module& m)
{

    using goertzel = ::gr::fft::goertzel;


    py::class_<goertzel, std::shared_ptr<goertzel>>(m, "goertzel", D(goertzel))

        .def(py::init<int, int, float>(),
             py::arg("rate"),
             py::arg("len"),
             py::arg("freq"),
             D(goertzel, goertzel, 0))
        .def(py::init<gr::fft::goertzel const&>(),
             py::arg("arg0"),
             D(goertzel, goertzel, 1))

        .def("set_params",
             &goertzel::set_params,
             py::arg("rate"),
             py::arg("len"),
             py::arg("freq"),
             D(goertzel, set_params))
        .def("batch", &goertzel::batch, py::arg("in"), D(goertzel, batch))
        .def("input", &goertzel::input, py::arg("in"), D(goertzel, input))


        .def("output", &goertzel::output, D(goertzel, output))


        .def("ready", &goertzel::ready, D(goertzel, ready))

        ;
}
