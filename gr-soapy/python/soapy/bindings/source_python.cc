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
/* BINDTOOL_HEADER_FILE(source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(1bc3c7623940898f7de621b3dfdeecc3)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/soapy/source.h>
// pydoc.h is automatically generated in the build directory
#include <source_pydoc.h>

void bind_source(py::module& m)
{

    using source = ::gr::soapy::source;


    py::class_<source,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<source>>(m, "source", D(source))

        .def(py::init(&source::make),
             py::arg("nchan"),
             py::arg("device"),
             py::arg("dev_args"),
             py::arg("stream_args"),
             py::arg("tune_args"),
             py::arg("other_settings"),
             py::arg("sampling_rate"),
             py::arg("type"),
             D(source, make))


        .def("get_antennas",
             &source::get_antennas,
             py::arg("channel"),
             D(source, get_antennas))


        .def("set_gain",
             (void (source::*)(size_t, float)) & source::set_gain,
             py::arg("channel"),
             py::arg("gain"),
             D(source, set_gain, 0))


        .def("set_gain",
             (void (source::*)(size_t, std::string const, float)) & source::set_gain,
             py::arg("channel"),
             py::arg("name"),
             py::arg("gain"),
             D(source, set_gain, 1))


        .def("set_frequency",
             (void (source::*)(size_t, double)) & source::set_frequency,
             py::arg("channel"),
             py::arg("freq"),
             D(source, set_frequency, 0))


        .def("set_frequency",
             (void (source::*)(size_t, std::string const&, double)) &
                 source::set_frequency,
             py::arg("channel"),
             py::arg("name"),
             py::arg("frequency"),
             D(source, set_frequency, 1))


        .def("set_agc",
             &source::set_agc,
             py::arg("channel"),
             py::arg("enable"),
             D(source, set_agc))


        .def("set_sample_rate",
             &source::set_sample_rate,
             py::arg("channel"),
             py::arg("sample_rate"),
             D(source, set_sample_rate))


        .def("set_bandwidth",
             &source::set_bandwidth,
             py::arg("channel"),
             py::arg("bandwidth"),
             D(source, set_bandwidth))


        .def("set_antenna",
             &source::set_antenna,
             py::arg("channel"),
             py::arg("name"),
             D(source, set_antenna))


        .def("set_dc_offset",
             &source::set_dc_offset,
             py::arg("channel"),
             py::arg("dc_offset"),
             D(source, set_dc_offset))


        .def("set_dc_removal",
             &source::set_dc_removal,
             py::arg("channel"),
             py::arg("automatic"),
             D(source, set_dc_removal))


        .def("set_frequency_correction",
             &source::set_frequency_correction,
             py::arg("channel"),
             py::arg("freq_correction"),
             D(source, set_frequency_correction))


        .def("set_iq_balance",
             &source::set_iq_balance,
             py::arg("channel"),
             py::arg("iq_balance"),
             D(source, set_iq_balance))


        .def("set_master_clock_rate",
             &source::set_master_clock_rate,
             py::arg("clock_rate"),
             D(source, set_master_clock_rate))


        .def("set_clock_source",
             &source::set_clock_source,
             py::arg("clock_source"),
             D(source, set_clock_source))

        ;
}
