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
/* BINDTOOL_HEADER_FILE(fmcomms2_sink_fc32.h)                                      */
/* BINDTOOL_HEADER_FILE_HASH(9f2679554149aa23caf1ccb4b0ddd02c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/iio/fmcomms2_sink_fc32.h>
// pydoc.h is automatically generated in the build directory
#include <fmcomms2_sink_f32c_pydoc.h>

void bind_fmcomms2_sink_f32c(py::module& m)
{

    using fmcomms2_sink_f32c = gr::iio::fmcomms2_sink_f32c;


    py::class_<fmcomms2_sink_f32c,
               gr::hier_block2,
               gr::basic_block,
               std::shared_ptr<fmcomms2_sink_f32c>>(
        m, "fmcomms2_sink_f32c", D(fmcomms2_sink_f32c))

        .def(py::init(&fmcomms2_sink_f32c::make),
             py::arg("uri"),
             py::arg("longfrequency"),
             py::arg("samplerate"),
             py::arg("bandwidth"),
             py::arg("tx1_en"),
             py::arg("tx2_en"),
             py::arg("buffer_size"),
             py::arg("cyclic"),
             py::arg("rf_port_select"),
             py::arg("attenuation1"),
             py::arg("attenuation2"),
             py::arg("filter_source") = "",
             py::arg("filter_filename") = "",
             py::arg("Fpass") = 0.0,
             py::arg("Fstop") = 0.0,
             D(fmcomms2_sink_f32c, make))

        .def("set_params",
             &fmcomms2_sink_f32c::set_params,
             py::arg("longfrequency"),
             py::arg("samplerate"),
             py::arg("bandwidth"),
             py::arg("rf_port_select"),
             py::arg("attenuation1"),
             py::arg("attenuation2"),
             py::arg("filter_source") = "",
             py::arg("filter_filename") = "",
             py::arg("Fpass") = 0.0,
             py::arg("Fstop") = 0.0,
             D(fmcomms2_sink_f32c, set_params))

        ;
}
