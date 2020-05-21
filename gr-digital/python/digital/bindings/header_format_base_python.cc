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
/* BINDTOOL_HEADER_FILE(header_format_base.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(5ae8266e0193a714bf51d99d89038dfb)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/header_format_base.h>
// pydoc.h is automatically generated in the build directory
#include <header_format_base_pydoc.h>

void bind_header_format_base(py::module& m)
{

    using header_format_base = ::gr::digital::header_format_base;


    py::class_<header_format_base, std::shared_ptr<header_format_base>>(
        m, "header_format_base", D(header_format_base))

        //.def(py::init<>(),D(header_format_base,header_format_base,0))
        //.def(py::init<gr::digital::header_format_base const &>(), py::arg("arg0"),
        //   D(header_format_base,header_format_base,1)
        //)


        .def("base", &header_format_base::base, D(header_format_base, base))


        .def(
            "formatter", &header_format_base::formatter, D(header_format_base, formatter))


        .def("format",
             &header_format_base::format,
             py::arg("nbytes_in"),
             py::arg("input"),
             py::arg("output"),
             py::arg("info"),
             D(header_format_base, format))
        .def("parse",
             &header_format_base::parse,
             py::arg("nbits_in"),
             py::arg("input"),
             py::arg("info"),
             py::arg("nbits_processed"),
             D(header_format_base, parse))


        .def("header_nbits",
             &header_format_base::header_nbits,
             D(header_format_base, header_nbits))


        .def("header_nbytes",
             &header_format_base::header_nbytes,
             D(header_format_base, header_nbytes))

        ;
}
