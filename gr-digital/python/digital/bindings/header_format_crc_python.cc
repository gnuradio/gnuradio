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
/* BINDTOOL_HEADER_FILE(header_format_crc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(6f463d9032ecfd4cba3fec81e9b2fcad)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/header_format_crc.h>
// pydoc.h is automatically generated in the build directory
#include <header_format_crc_pydoc.h>

void bind_header_format_crc(py::module& m)
{

    using header_format_crc = ::gr::digital::header_format_crc;

    py::class_<header_format_crc,
               gr::digital::header_format_base,
               std::shared_ptr<header_format_crc>>(
        m, "header_format_crc", D(header_format_crc))


        .def(py::init(&header_format_crc::make),
             py::arg("len_key_name") = "packet_len",
             py::arg("num_key_name") = "packet_num",
             D(header_format_crc, make))
        .def_static("make",
                    &header_format_crc::make,
                    py::arg("len_key_name") = "packet_len",
                    py::arg("num_key_name") = "packet_num",
                    D(header_format_crc, make))


        .def("set_header_num",
             &header_format_crc::set_header_num,
             py::arg("header_num"),
             D(header_format_crc, set_header_num))
        .def("format",
             &header_format_crc::format,
             py::arg("nbytes_in"),
             py::arg("input"),
             py::arg("output"),
             py::arg("info"),
             D(header_format_crc, format))
        .def("parse",
             &header_format_crc::parse,
             py::arg("nbits_in"),
             py::arg("input"),
             py::arg("info"),
             py::arg("nbits_processed"),
             D(header_format_crc, parse))


        .def("header_nbits",
             &header_format_crc::header_nbits,
             D(header_format_crc, header_nbits))


        ;
}
