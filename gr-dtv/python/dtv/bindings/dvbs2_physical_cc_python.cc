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
/* BINDTOOL_GEN_AUTOMATIC(1)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(dvbs2_physical_cc.h)                                       */
/* BINDTOOL_HEADER_FILE_HASH(daa4f75e5b420ca14e4b7245b4328d87)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/dtv/dvbs2_physical_cc.h>
// pydoc.h is automatically generated in the build directory
#include <dvbs2_physical_cc_pydoc.h>

void bind_dvbs2_physical_cc(py::module& m)
{

    using dvbs2_physical_cc = ::gr::dtv::dvbs2_physical_cc;


    py::class_<dvbs2_physical_cc,
               gr::block,
               gr::basic_block,
               std::shared_ptr<dvbs2_physical_cc>>(
        m, "dvbs2_physical_cc", D(dvbs2_physical_cc))

        .def(py::init(&dvbs2_physical_cc::make),
             py::arg("framesize"),
             py::arg("rate"),
             py::arg("constellation"),
             py::arg("pilots"),
             py::arg("goldcode"),
             D(dvbs2_physical_cc, make))


        ;
}
