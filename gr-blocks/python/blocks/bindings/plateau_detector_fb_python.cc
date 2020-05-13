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
/* BINDTOOL_HEADER_FILE(plateau_detector_fb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(8602ec9c626ae1fcba3aa2481f31dfd1)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/plateau_detector_fb.h>
// pydoc.h is automatically generated in the build directory
#include <plateau_detector_fb_pydoc.h>

void bind_plateau_detector_fb(py::module& m)
{

    using plateau_detector_fb = ::gr::blocks::plateau_detector_fb;


    py::class_<plateau_detector_fb,
               gr::block,
               gr::basic_block,
               std::shared_ptr<plateau_detector_fb>>(
        m, "plateau_detector_fb", D(plateau_detector_fb))

        .def(py::init(&plateau_detector_fb::make),
             py::arg("max_len"),
             py::arg("threshold") = 0.90000000000000002,
             D(plateau_detector_fb, make))


        .def("set_threshold",
             &plateau_detector_fb::set_threshold,
             py::arg("threshold"),
             D(plateau_detector_fb, set_threshold))


        .def("threshold",
             &plateau_detector_fb::threshold,
             D(plateau_detector_fb, threshold))

        ;
}
