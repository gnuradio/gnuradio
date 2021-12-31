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
/* BINDTOOL_HEADER_FILE(transcendental.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(277c7a6a382c60bf340be71588f512da)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/transcendental.h>
// pydoc.h is automatically generated in the build directory
#include <transcendental_pydoc.h>

void bind_transcendental(py::module& m)
{

    using transcendental = ::gr::blocks::transcendental;


    py::class_<transcendental,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<transcendental>>(m, "transcendental", D(transcendental))

        .def(py::init(&transcendental::make),
             py::arg("name"),
             py::arg("type") = "float",
             D(transcendental, make))


        ;
}
