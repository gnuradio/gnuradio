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
/* BINDTOOL_HEADER_FILE(pfb_arb_resampler_ccf.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(9da2cd9fb2e5cac083d0ce763e189351)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/pfb_arb_resampler_ccf.h>
// pydoc.h is automatically generated in the build directory
#include <pfb_arb_resampler_ccf_pydoc.h>

void bind_pfb_arb_resampler_ccf(py::module& m)
{

    using pfb_arb_resampler_ccf = ::gr::filter::pfb_arb_resampler_ccf;


    py::class_<pfb_arb_resampler_ccf,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pfb_arb_resampler_ccf>>(
        m, "pfb_arb_resampler_ccf", D(pfb_arb_resampler_ccf))

        .def(py::init(&pfb_arb_resampler_ccf::make),
             py::arg("rate"),
             py::arg("taps"),
             py::arg("filter_size") = 32,
             D(pfb_arb_resampler_ccf, make))


        .def("set_taps",
             &pfb_arb_resampler_ccf::set_taps,
             py::arg("taps"),
             D(pfb_arb_resampler_ccf, set_taps))


        .def("taps", &pfb_arb_resampler_ccf::taps, D(pfb_arb_resampler_ccf, taps))


        .def("print_taps",
             &pfb_arb_resampler_ccf::print_taps,
             D(pfb_arb_resampler_ccf, print_taps))


        .def("set_rate",
             &pfb_arb_resampler_ccf::set_rate,
             py::arg("rate"),
             D(pfb_arb_resampler_ccf, set_rate))


        .def("set_phase",
             &pfb_arb_resampler_ccf::set_phase,
             py::arg("ph"),
             D(pfb_arb_resampler_ccf, set_phase))


        .def("phase", &pfb_arb_resampler_ccf::phase, D(pfb_arb_resampler_ccf, phase))


        .def("taps_per_filter",
             &pfb_arb_resampler_ccf::taps_per_filter,
             D(pfb_arb_resampler_ccf, taps_per_filter))


        .def("interpolation_rate",
             &pfb_arb_resampler_ccf::interpolation_rate,
             D(pfb_arb_resampler_ccf, interpolation_rate))


        .def("decimation_rate",
             &pfb_arb_resampler_ccf::decimation_rate,
             D(pfb_arb_resampler_ccf, decimation_rate))


        .def("fractional_rate",
             &pfb_arb_resampler_ccf::fractional_rate,
             D(pfb_arb_resampler_ccf, fractional_rate))


        .def("group_delay",
             &pfb_arb_resampler_ccf::group_delay,
             D(pfb_arb_resampler_ccf, group_delay))


        .def("phase_offset",
             &pfb_arb_resampler_ccf::phase_offset,
             py::arg("freq"),
             py::arg("fs"),
             D(pfb_arb_resampler_ccf, phase_offset))

        ;
}
