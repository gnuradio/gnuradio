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
/* BINDTOOL_HEADER_FILE(nop.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(456e7e6381fbc3e64a2b041aab4402bc)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/nop.h>
// pydoc.h is automatically generated in the build directory
#include <nop_pydoc.h>

void bind_nop(py::module& m)
{

    using nop = ::gr::blocks::nop;


    py::class_<nop, gr::block, gr::basic_block, std::shared_ptr<nop>>(m, "nop", D(nop))

        .def(py::init(&nop::make), py::arg("sizeof_stream_item"), D(nop, make))


        .def("nmsgs_received", &nop::nmsgs_received, D(nop, nmsgs_received))


        .def("ctrlport_test", &nop::ctrlport_test, D(nop, ctrlport_test))


        .def("set_ctrlport_test",
             &nop::set_ctrlport_test,
             py::arg("x"),
             D(nop, set_ctrlport_test))

        ;
}
