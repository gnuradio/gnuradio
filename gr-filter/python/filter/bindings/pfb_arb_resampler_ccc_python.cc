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
/* BINDTOOL_HEADER_FILE(pfb_arb_resampler_ccc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(3a3d9e216f320972e07be5f798e6be4e)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/pfb_arb_resampler_ccc.h>
// pydoc.h is automatically generated in the build directory
#include <pfb_arb_resampler_ccc_pydoc.h>

void bind_pfb_arb_resampler_ccc(py::module& m)
{

    using pfb_arb_resampler_ccc = ::gr::filter::pfb_arb_resampler_ccc;


    py::class_<pfb_arb_resampler_ccc,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pfb_arb_resampler_ccc>>(
        m, "pfb_arb_resampler_ccc", D(pfb_arb_resampler_ccc))

        .def(py::init(&pfb_arb_resampler_ccc::make),
             py::arg("rate"),
             py::arg("taps"),
             py::arg("filter_size") = 32,
             D(pfb_arb_resampler_ccc, make))


        .def("set_taps",
             &pfb_arb_resampler_ccc::set_taps,
             py::arg("taps"),
             D(pfb_arb_resampler_ccc, set_taps))


        .def("taps", &pfb_arb_resampler_ccc::taps, D(pfb_arb_resampler_ccc, taps))


        .def("print_taps",
             &pfb_arb_resampler_ccc::print_taps,
             D(pfb_arb_resampler_ccc, print_taps))


        .def("set_rate",
             &pfb_arb_resampler_ccc::set_rate,
             py::arg("rate"),
             D(pfb_arb_resampler_ccc, set_rate))


        .def("set_phase",
             &pfb_arb_resampler_ccc::set_phase,
             py::arg("ph"),
             D(pfb_arb_resampler_ccc, set_phase))


        .def("phase", &pfb_arb_resampler_ccc::phase, D(pfb_arb_resampler_ccc, phase))


        .def("taps_per_filter",
             &pfb_arb_resampler_ccc::taps_per_filter,
             D(pfb_arb_resampler_ccc, taps_per_filter))


        .def("interpolation_rate",
             &pfb_arb_resampler_ccc::interpolation_rate,
             D(pfb_arb_resampler_ccc, interpolation_rate))


        .def("decimation_rate",
             &pfb_arb_resampler_ccc::decimation_rate,
             D(pfb_arb_resampler_ccc, decimation_rate))


        .def("fractional_rate",
             &pfb_arb_resampler_ccc::fractional_rate,
             D(pfb_arb_resampler_ccc, fractional_rate))


        .def("group_delay",
             &pfb_arb_resampler_ccc::group_delay,
             D(pfb_arb_resampler_ccc, group_delay))


        .def("phase_offset",
             &pfb_arb_resampler_ccc::phase_offset,
             py::arg("freq"),
             py::arg("fs"),
             D(pfb_arb_resampler_ccc, phase_offset))

        ;
}
