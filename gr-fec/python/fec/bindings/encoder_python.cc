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
/* BINDTOOL_HEADER_FILE(encoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(789613fcb6f75ec11cd06bfd196ea79d)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec/encoder.h>
// pydoc.h is automatically generated in the build directory
#include <encoder_pydoc.h>

void bind_encoder(py::module& m)
{

    using encoder = ::gr::fec::encoder;


    py::class_<encoder, gr::block, gr::basic_block, std::shared_ptr<encoder>>(
        m, "encoder", D(encoder))

        .def(py::init(&encoder::make),
             py::arg("my_encoder"),
             py::arg("input_item_size"),
             py::arg("output_item_size"),
             D(encoder, make))


        .def("general_work",
             &encoder::general_work,
             py::arg("noutput_items"),
             py::arg("ninput_items"),
             py::arg("input_items"),
             py::arg("output_items"),
             D(encoder, general_work))


        .def("fixed_rate_ninput_to_noutput",
             &encoder::fixed_rate_ninput_to_noutput,
             py::arg("ninput"),
             D(encoder, fixed_rate_ninput_to_noutput))


        .def("fixed_rate_noutput_to_ninput",
             &encoder::fixed_rate_noutput_to_ninput,
             py::arg("noutput"),
             D(encoder, fixed_rate_noutput_to_ninput))

        ;
}
