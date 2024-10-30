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
/* BINDTOOL_HEADER_FILE(source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(14be022ac6d9db93a977f3e27a2f1550)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/audio/source.h>
// pydoc.h is automatically generated in the build directory
#include <source_pydoc.h>

void bind_source(py::module& m)
{

    using source = ::gr::audio::source;


    py::class_<source,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<source>>(m, "source", D(source))

        .def(py::init(&source::make),
             py::arg("sampling_rate"),
             py::arg("device_name") = "",
             py::arg("ok_to_block") = true,
             D(source, make))


        ;
}
