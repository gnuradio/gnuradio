/* -*- c++ -*- */
/*
 * Copyright 2020-2021 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(sink.h)                                                    */
/* BINDTOOL_HEADER_FILE_HASH(07d0b08af83183ae93f5783b1f0e2757)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/soapy/block.h>
#include <gnuradio/soapy/sink.h>
// pydoc.h is automatically generated in the build directory
#include <sink_pydoc.h>

void bind_sink(py::module& m)
{

    using sink = ::gr::soapy::sink;


    py::class_<sink, gr::soapy::block, gr::block, gr::basic_block, std::shared_ptr<sink>>(
        m, "sink", D(sink))

        .def(py::init(&sink::make),
             py::arg("device"),
             py::arg("type"),
             py::arg("nchan"),
             py::arg("dev_args"),
             py::arg("stream_args"),
             py::arg("tune_args"),
             py::arg("other_settings"),
             D(sink, make))

        .def("set_length_tag_name",
             &sink::set_length_tag_name,
             py::arg("length_tag_name"),
             D(sink, set_length_tag_name));
}
