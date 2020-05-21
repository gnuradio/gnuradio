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
/* BINDTOOL_HEADER_FILE(iir_filter_ccf.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(8df6c0a50f429e59bd4903886da28ba2)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/iir_filter_ccf.h>
// pydoc.h is automatically generated in the build directory
#include <iir_filter_ccf_pydoc.h>

void bind_iir_filter_ccf(py::module& m)
{

    using iir_filter_ccf = ::gr::filter::iir_filter_ccf;


    py::class_<iir_filter_ccf,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<iir_filter_ccf>>(m, "iir_filter_ccf", D(iir_filter_ccf))

        .def(py::init(&iir_filter_ccf::make),
             py::arg("fftaps"),
             py::arg("fbtaps"),
             py::arg("oldstyle") = true,
             D(iir_filter_ccf, make))


        .def("set_taps",
             &iir_filter_ccf::set_taps,
             py::arg("fftaps"),
             py::arg("fbtaps"),
             D(iir_filter_ccf, set_taps))

        ;
}
