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
/* BINDTOOL_HEADER_FILE(control_loop.h)                                            */
/* BINDTOOL_HEADER_FILE_HASH(2136814678ebb25b6a18e6de1aaa4eb7)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/control_loop.h>
// pydoc.h is automatically generated in the build directory
#include <control_loop_pydoc.h>

void bind_control_loop(py::module& m)
{

    using control_loop = ::gr::blocks::control_loop;


    py::class_<control_loop, std::shared_ptr<control_loop>>(
        m, "control_loop", D(control_loop))

        .def(py::init<>(), D(control_loop, control_loop, 0))
        .def(py::init<float, float, float>(),
             py::arg("loop_bw"),
             py::arg("max_freq"),
             py::arg("min_freq"),
             D(control_loop, control_loop, 1))
        .def(py::init<gr::blocks::control_loop const&>(),
             py::arg("arg0"),
             D(control_loop, control_loop, 2))


        .def("update_gains", &control_loop::update_gains, D(control_loop, update_gains))


        .def("advance_loop",
             &control_loop::advance_loop,
             py::arg("error"),
             D(control_loop, advance_loop))


        .def("phase_wrap", &control_loop::phase_wrap, D(control_loop, phase_wrap))


        .def("frequency_limit",
             &control_loop::frequency_limit,
             D(control_loop, frequency_limit))


        .def("set_loop_bandwidth",
             &control_loop::set_loop_bandwidth,
             py::arg("bw"),
             D(control_loop, set_loop_bandwidth))


        .def("set_damping_factor",
             &control_loop::set_damping_factor,
             py::arg("df"),
             D(control_loop, set_damping_factor))


        .def("set_alpha",
             &control_loop::set_alpha,
             py::arg("alpha"),
             D(control_loop, set_alpha))


        .def("set_beta",
             &control_loop::set_beta,
             py::arg("beta"),
             D(control_loop, set_beta))


        .def("set_frequency",
             &control_loop::set_frequency,
             py::arg("freq"),
             D(control_loop, set_frequency))


        .def("set_phase",
             &control_loop::set_phase,
             py::arg("phase"),
             D(control_loop, set_phase))


        .def("set_max_freq",
             &control_loop::set_max_freq,
             py::arg("freq"),
             D(control_loop, set_max_freq))


        .def("set_min_freq",
             &control_loop::set_min_freq,
             py::arg("freq"),
             D(control_loop, set_min_freq))


        .def("get_loop_bandwidth",
             &control_loop::get_loop_bandwidth,
             D(control_loop, get_loop_bandwidth))


        .def("get_damping_factor",
             &control_loop::get_damping_factor,
             D(control_loop, get_damping_factor))


        .def("get_alpha", &control_loop::get_alpha, D(control_loop, get_alpha))


        .def("get_beta", &control_loop::get_beta, D(control_loop, get_beta))


        .def(
            "get_frequency", &control_loop::get_frequency, D(control_loop, get_frequency))


        .def("get_phase", &control_loop::get_phase, D(control_loop, get_phase))


        .def("get_max_freq", &control_loop::get_max_freq, D(control_loop, get_max_freq))


        .def("get_min_freq", &control_loop::get_min_freq, D(control_loop, get_min_freq))

        ;


    m.def("tanhf_lut", &::gr::blocks::tanhf_lut, py::arg("x"), D(tanhf_lut));
}
