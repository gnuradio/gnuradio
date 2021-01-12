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
/* BINDTOOL_HEADER_FILE(fxpt_nco.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a464f7bdf49d3dfafc989a2b3f4d7b94)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fxpt_nco.h>
// pydoc.h is automatically generated in the build directory
#include <fxpt_nco_pydoc.h>

void bind_fxpt_nco(py::module& m)
{

    using fxpt_nco = ::gr::fxpt_nco;


    py::class_<fxpt_nco, std::shared_ptr<fxpt_nco>>(m, "fxpt_nco", D(fxpt_nco))

        .def(py::init<>(), D(fxpt_nco, fxpt_nco, 0))
        .def(py::init<gr::fxpt_nco const&>(), py::arg("arg0"), D(fxpt_nco, fxpt_nco, 1))


        .def("set_phase", &fxpt_nco::set_phase, py::arg("angle"), D(fxpt_nco, set_phase))


        .def("adjust_phase",
             &fxpt_nco::adjust_phase,
             py::arg("delta_phase"),
             D(fxpt_nco, adjust_phase))


        .def(
            "set_freq", &fxpt_nco::set_freq, py::arg("angle_rate"), D(fxpt_nco, set_freq))


        .def("adjust_freq",
             &fxpt_nco::adjust_freq,
             py::arg("delta_angle_rate"),
             D(fxpt_nco, adjust_freq))


        .def("step", (void (fxpt_nco::*)()) & fxpt_nco::step, D(fxpt_nco, step, 0))


        .def("step",
             (void (fxpt_nco::*)(int)) & fxpt_nco::step,
             py::arg("n"),
             D(fxpt_nco, step, 1))


        .def("get_phase", &fxpt_nco::get_phase, D(fxpt_nco, get_phase))


        .def("get_freq", &fxpt_nco::get_freq, D(fxpt_nco, get_freq))


        .def("sincos",
             (void (fxpt_nco::*)(float*, float*) const) & fxpt_nco::sincos,
             py::arg("sinx"),
             py::arg("cosx"),
             D(fxpt_nco, sincos, 0))


        .def("sincos",
             (void (fxpt_nco::*)(gr_complex*, int, double)) & fxpt_nco::sincos,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, sincos, 1))


        .def("sin",
             (void (fxpt_nco::*)(float*, int, double)) & fxpt_nco::sin,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, sin, 0))


        .def("cos",
             (void (fxpt_nco::*)(float*, int, double)) & fxpt_nco::cos,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, cos, 0))


        .def("sin",
             (void (fxpt_nco::*)(int8_t*, int, double)) & fxpt_nco::sin,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, sin, 1))


        .def("cos",
             (void (fxpt_nco::*)(int8_t*, int, double)) & fxpt_nco::cos,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, cos, 1))


        .def("sin",
             (void (fxpt_nco::*)(short int*, int, double)) & fxpt_nco::sin,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, sin, 2))


        .def("cos",
             (void (fxpt_nco::*)(short int*, int, double)) & fxpt_nco::cos,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, cos, 2))


        .def("sin",
             (void (fxpt_nco::*)(int*, int, double)) & fxpt_nco::sin,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, sin, 3))


        .def("cos",
             (void (fxpt_nco::*)(int*, int, double)) & fxpt_nco::cos,
             py::arg("output"),
             py::arg("noutput_items"),
             py::arg("ampl") = 1.,
             D(fxpt_nco, cos, 3))


        .def("cos", (float (fxpt_nco::*)() const) & fxpt_nco::cos, D(fxpt_nco, cos, 4))


        .def("sin", (float (fxpt_nco::*)() const) & fxpt_nco::sin, D(fxpt_nco, sin, 4))

        ;
}
