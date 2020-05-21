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
/* BINDTOOL_HEADER_FILE(fec_mtrx.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(80c2f34bc9bdb9ac03abdf0c26b3d024)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fec/fec_mtrx.h>
// pydoc.h is automatically generated in the build directory
#include <fec_mtrx_pydoc.h>

void bind_fec_mtrx(py::module& m)
{


    py::module m_code = m.def_submodule("code");

    using block_data = ::gr::fec::code::block_data;
    using matrix = ::gr::fec::code::matrix;
    using fec_mtrx = ::gr::fec::code::fec_mtrx;


    py::class_<block_data, std::shared_ptr<block_data>>(
        m_code, "block_data", D(code, block_data))


        ;

    py::class_<matrix, std::shared_ptr<matrix>>(m_code, "matrix", D(code, matrix))


        ;


    py::class_<fec_mtrx, std::shared_ptr<fec_mtrx>>(m_code, "fec_mtrx", D(code, fec_mtrx))

        // .def(py::init<gr::fec::code::fec_mtrx const&>(),
        //      py::arg("arg0"),
        //      D(code, fec_mtrx, fec_mtrx))


        .def("encode",
             &fec_mtrx::encode,
             py::arg("outbuffer"),
             py::arg("inbuffer"),
             D(code, fec_mtrx, encode))


        .def("decode",
             &fec_mtrx::decode,
             py::arg("outbuffer"),
             py::arg("inbuffer"),
             py::arg("frame_size"),
             py::arg("max_iterations"),
             D(code, fec_mtrx, decode))


        .def("n", &fec_mtrx::n, D(code, fec_mtrx, n))


        .def("k", &fec_mtrx::k, D(code, fec_mtrx, k))

        ;


    m_code.def(
        "matrix_free", &::gr::fec::code::matrix_free, py::arg("x"), D(code, matrix_free));


    m_code.def("read_matrix_from_file",
               &::gr::fec::code::read_matrix_from_file,
               py::arg("filename"),
               D(code, read_matrix_from_file));


    m_code.def("write_matrix_to_file",
               &::gr::fec::code::write_matrix_to_file,
               py::arg("filename"),
               py::arg("M"),
               D(code, write_matrix_to_file));


    m_code.def("generate_G_transpose",
               &::gr::fec::code::generate_G_transpose,
               py::arg("H_obj"),
               D(code, generate_G_transpose));


    m_code.def("generate_G",
               &::gr::fec::code::generate_G,
               py::arg("H_obj"),
               D(code, generate_G));


    m_code.def("generate_H",
               &::gr::fec::code::generate_H,
               py::arg("G_obj"),
               D(code, generate_H));


    m_code.def("print_matrix",
               &::gr::fec::code::print_matrix,
               py::arg("M"),
               py::arg("numpy") = false,
               D(code, print_matrix));
}
