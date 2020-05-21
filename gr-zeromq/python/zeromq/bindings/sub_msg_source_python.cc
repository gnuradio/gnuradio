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
/* BINDTOOL_HEADER_FILE(sub_msg_source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(35c48e1b4d8719899d4142620ff7735d)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/zeromq/sub_msg_source.h>
// pydoc.h is automatically generated in the build directory
#include <sub_msg_source_pydoc.h>

void bind_sub_msg_source(py::module& m)
{

    using sub_msg_source = ::gr::zeromq::sub_msg_source;


    py::class_<sub_msg_source,
               gr::block,
               gr::basic_block,
               std::shared_ptr<sub_msg_source>>(m, "sub_msg_source", D(sub_msg_source))

        .def(py::init(&sub_msg_source::make),
             py::arg("address"),
             py::arg("timeout") = 100,
             py::arg("bind") = false,
             D(sub_msg_source, make))


        .def("last_endpoint",
             &sub_msg_source::last_endpoint,
             D(sub_msg_source, last_endpoint))

        ;
}
