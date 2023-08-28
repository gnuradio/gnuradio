/*
 * Copyright 2023 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(header_buffer.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c8ccc58a0efc5332b29a6fa9c2f0e828)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/header_buffer.h>
// pydoc.h is automatically generated in the build directory
#include <header_buffer_pydoc.h>

void bind_header_buffer(py::module& m)
{

    using header_buffer = ::gr::digital::header_buffer;


    py::class_<header_buffer, std::shared_ptr<header_buffer>>(
        m, "header_buffer", D(header_buffer))

        .def(py::init<uint8_t*>(),
             py::arg("buffer") = nullptr,
             D(header_buffer, header_buffer, 0))
        .def(py::init<gr::digital::header_buffer const&>(),
             py::arg("arg0"),
             D(header_buffer, header_buffer, 1))


        .def("clear", &header_buffer::clear, D(header_buffer, clear))


        .def("header", &header_buffer::header, D(header_buffer, header))


        .def("add_field8",
             &header_buffer::add_field8,
             py::arg("data"),
             py::arg("len") = 8,
             py::arg("bs") = false,
             D(header_buffer, add_field8))


        .def("add_field16",
             &header_buffer::add_field16,
             py::arg("data"),
             py::arg("len") = 16,
             py::arg("bs") = false,
             D(header_buffer, add_field16))


        .def("add_field32",
             &header_buffer::add_field32,
             py::arg("data"),
             py::arg("len") = 32,
             py::arg("bs") = false,
             D(header_buffer, add_field32))


        .def("add_field64",
             &header_buffer::add_field64,
             py::arg("data"),
             py::arg("len") = 64,
             py::arg("bs") = false,
             D(header_buffer, add_field64))


        .def("insert_bit",
             &header_buffer::insert_bit,
             py::arg("bit"),
             D(header_buffer, insert_bit))


        .def("extract_field8",
             &header_buffer::extract_field8,
             py::arg("pos"),
             py::arg("len") = 8,
             py::arg("bs") = false,
             py::arg("lsb_first") = false,
             D(header_buffer, extract_field8))


        .def("extract_field16",
             &header_buffer::extract_field16,
             py::arg("pos"),
             py::arg("len") = 16,
             py::arg("bs") = false,
             py::arg("lsb_first") = false,
             D(header_buffer, extract_field16))


        .def("extract_field32",
             &header_buffer::extract_field32,
             py::arg("pos"),
             py::arg("len") = 32,
             py::arg("bs") = false,
             py::arg("lsb_first") = false,
             D(header_buffer, extract_field32))


        .def("extract_field64",
             &header_buffer::extract_field64,
             py::arg("pos"),
             py::arg("len") = 64,
             py::arg("bs") = false,
             py::arg("lsb_first") = false,
             D(header_buffer, extract_field64))

        ;
}
