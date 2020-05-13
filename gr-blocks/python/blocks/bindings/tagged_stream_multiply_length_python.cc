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
/* BINDTOOL_HEADER_FILE(tagged_stream_multiply_length.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(d35538f07d2fbd15d2b08a39de2090b3)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/tagged_stream_multiply_length.h>
// pydoc.h is automatically generated in the build directory
#include <tagged_stream_multiply_length_pydoc.h>

void bind_tagged_stream_multiply_length(py::module& m)
{

    using tagged_stream_multiply_length = ::gr::blocks::tagged_stream_multiply_length;


    py::class_<tagged_stream_multiply_length,
               gr::block,
               gr::basic_block,
               std::shared_ptr<tagged_stream_multiply_length>>(
        m, "tagged_stream_multiply_length", D(tagged_stream_multiply_length))

        .def(py::init(&tagged_stream_multiply_length::make),
             py::arg("itemsize"),
             py::arg("lengthtagname"),
             py::arg("scalar"),
             D(tagged_stream_multiply_length, make))


        .def("set_scalar",
             &tagged_stream_multiply_length::set_scalar,
             py::arg("scalar"),
             D(tagged_stream_multiply_length, set_scalar))


        ;
}
