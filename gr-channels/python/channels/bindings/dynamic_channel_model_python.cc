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
/* BINDTOOL_HEADER_FILE(dynamic_channel_model.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(6d8a7c13d8e2b007e9d5f23e6e5749eb)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/channels/dynamic_channel_model.h>
// pydoc.h is automatically generated in the build directory
#include <dynamic_channel_model_pydoc.h>

void bind_dynamic_channel_model(py::module& m)
{

    using dynamic_channel_model = ::gr::channels::dynamic_channel_model;


    py::class_<dynamic_channel_model,
               gr::hier_block2,
               gr::basic_block,
               std::shared_ptr<dynamic_channel_model>>(
        m, "dynamic_channel_model", D(dynamic_channel_model))

        .def(py::init(&dynamic_channel_model::make),
             py::arg("samp_rate"),
             py::arg("sro_std_dev"),
             py::arg("sro_max_dev"),
             py::arg("cfo_std_dev"),
             py::arg("cfo_max_dev"),
             py::arg("N"),
             py::arg("doppler_freq"),
             py::arg("LOS_model"),
             py::arg("K"),
             py::arg("delays"),
             py::arg("mags"),
             py::arg("ntaps_mpath"),
             py::arg("noise_amp"),
             py::arg("noise_seed"),
             D(dynamic_channel_model, make))


        .def("samp_rate",
             &dynamic_channel_model::samp_rate,
             D(dynamic_channel_model, samp_rate))


        .def("sro_dev_std",
             &dynamic_channel_model::sro_dev_std,
             D(dynamic_channel_model, sro_dev_std))


        .def("sro_dev_max",
             &dynamic_channel_model::sro_dev_max,
             D(dynamic_channel_model, sro_dev_max))


        .def("cfo_dev_std",
             &dynamic_channel_model::cfo_dev_std,
             D(dynamic_channel_model, cfo_dev_std))


        .def("cfo_dev_max",
             &dynamic_channel_model::cfo_dev_max,
             D(dynamic_channel_model, cfo_dev_max))


        .def("noise_amp",
             &dynamic_channel_model::noise_amp,
             D(dynamic_channel_model, noise_amp))


        .def("doppler_freq",
             &dynamic_channel_model::doppler_freq,
             D(dynamic_channel_model, doppler_freq))


        .def("K", &dynamic_channel_model::K, D(dynamic_channel_model, K))


        .def("set_samp_rate",
             &dynamic_channel_model::set_samp_rate,
             py::arg("arg0"),
             D(dynamic_channel_model, set_samp_rate))


        .def("set_sro_dev_std",
             &dynamic_channel_model::set_sro_dev_std,
             py::arg("arg0"),
             D(dynamic_channel_model, set_sro_dev_std))


        .def("set_sro_dev_max",
             &dynamic_channel_model::set_sro_dev_max,
             py::arg("arg0"),
             D(dynamic_channel_model, set_sro_dev_max))


        .def("set_cfo_dev_std",
             &dynamic_channel_model::set_cfo_dev_std,
             py::arg("arg0"),
             D(dynamic_channel_model, set_cfo_dev_std))


        .def("set_cfo_dev_max",
             &dynamic_channel_model::set_cfo_dev_max,
             py::arg("arg0"),
             D(dynamic_channel_model, set_cfo_dev_max))


        .def("set_noise_amp",
             &dynamic_channel_model::set_noise_amp,
             py::arg("arg0"),
             D(dynamic_channel_model, set_noise_amp))


        .def("set_doppler_freq",
             &dynamic_channel_model::set_doppler_freq,
             py::arg("arg0"),
             D(dynamic_channel_model, set_doppler_freq))


        .def("set_K",
             &dynamic_channel_model::set_K,
             py::arg("arg0"),
             D(dynamic_channel_model, set_K))

        ;
}
