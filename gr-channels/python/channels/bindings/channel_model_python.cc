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
/* BINDTOOL_HEADER_FILE(channel_model.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c9343a196acb27614ee79886a9f24e20)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/channels/channel_model.h>
// pydoc.h is automatically generated in the build directory
#include <channel_model_pydoc.h>

void bind_channel_model(py::module& m)
{

    using channel_model = ::gr::channels::channel_model;


    py::class_<channel_model,
               gr::hier_block2,
               gr::basic_block,
               std::shared_ptr<channel_model>>(m, "channel_model", D(channel_model))

        .def(py::init(&channel_model::make),
             py::arg("noise_voltage") = 0.,
             py::arg("frequency_offset") = 0.,
             py::arg("epsilon") = 1.,
             py::arg("taps") = std::vector<gr_complex>(1, 1),
             py::arg("noise_seed") = 0,
             py::arg("block_tags") = false,
             D(channel_model, make))


        .def("set_noise_voltage",
             &channel_model::set_noise_voltage,
             py::arg("noise_voltage"),
             D(channel_model, set_noise_voltage))


        .def("set_frequency_offset",
             &channel_model::set_frequency_offset,
             py::arg("frequency_offset"),
             D(channel_model, set_frequency_offset))


        .def("set_taps",
             &channel_model::set_taps,
             py::arg("taps"),
             D(channel_model, set_taps))


        .def("set_timing_offset",
             &channel_model::set_timing_offset,
             py::arg("epsilon"),
             D(channel_model, set_timing_offset))


        .def("noise_voltage",
             &channel_model::noise_voltage,
             D(channel_model, noise_voltage))


        .def("frequency_offset",
             &channel_model::frequency_offset,
             D(channel_model, frequency_offset))


        .def("taps", &channel_model::taps, D(channel_model, taps))


        .def("timing_offset",
             &channel_model::timing_offset,
             D(channel_model, timing_offset))

        ;
}
