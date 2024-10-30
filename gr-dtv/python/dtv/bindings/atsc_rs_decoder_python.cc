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
/* BINDTOOL_GEN_AUTOMATIC(1)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(atsc_rs_decoder.h)                                         */
/* BINDTOOL_HEADER_FILE_HASH(707ab89ae43233b24eb629751691cbb6)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/dtv/atsc_rs_decoder.h>
// pydoc.h is automatically generated in the build directory
#include <atsc_rs_decoder_pydoc.h>

void bind_atsc_rs_decoder(py::module& m)
{

    using atsc_rs_decoder = ::gr::dtv::atsc_rs_decoder;


    py::class_<atsc_rs_decoder,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<atsc_rs_decoder>>(m, "atsc_rs_decoder", D(atsc_rs_decoder))

        .def(py::init(&atsc_rs_decoder::make), D(atsc_rs_decoder, make))


        .def("num_errors_corrected",
             &atsc_rs_decoder::num_errors_corrected,
             D(atsc_rs_decoder, num_errors_corrected))


        .def("num_bad_packets",
             &atsc_rs_decoder::num_bad_packets,
             D(atsc_rs_decoder, num_bad_packets))


        .def(
            "num_packets", &atsc_rs_decoder::num_packets, D(atsc_rs_decoder, num_packets))


        ;
}
