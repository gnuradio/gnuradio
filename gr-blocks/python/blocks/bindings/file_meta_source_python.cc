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
/* BINDTOOL_HEADER_FILE(file_meta_source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(bc83380d48d928da1243a78ce24cb0d8)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/file_meta_source.h>
// pydoc.h is automatically generated in the build directory
#include <file_meta_source_pydoc.h>

void bind_file_meta_source(py::module& m)
{

    using file_meta_source = ::gr::blocks::file_meta_source;


    py::class_<file_meta_source,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<file_meta_source>>(
        m, "file_meta_source", D(file_meta_source))

        .def(py::init(&file_meta_source::make),
             py::arg("filename"),
             py::arg("repeat") = false,
             py::arg("detached_header") = false,
             py::arg("hdr_filename") = "",
             D(file_meta_source, make))


        .def("open",
             &file_meta_source::open,
             py::arg("filename"),
             py::arg("hdr_filename") = "",
             D(file_meta_source, open))


        .def("close", &file_meta_source::close, D(file_meta_source, close))


        .def("do_update", &file_meta_source::do_update, D(file_meta_source, do_update))

        ;
}
