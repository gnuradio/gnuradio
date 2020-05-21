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
/* BINDTOOL_HEADER_FILE(pn_correlator_cc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(9553e73d637259cb0b60ebab0c4aed3e)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/pn_correlator_cc.h>
// pydoc.h is automatically generated in the build directory
#include <pn_correlator_cc_pydoc.h>

void bind_pn_correlator_cc(py::module& m)
{

    using pn_correlator_cc = ::gr::digital::pn_correlator_cc;


    py::class_<pn_correlator_cc,
               gr::sync_decimator,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pn_correlator_cc>>(
        m, "pn_correlator_cc", D(pn_correlator_cc))

        .def(py::init(&pn_correlator_cc::make),
             py::arg("degree"),
             py::arg("mask") = 0,
             py::arg("seed") = 1,
             D(pn_correlator_cc, make))


        ;
}
