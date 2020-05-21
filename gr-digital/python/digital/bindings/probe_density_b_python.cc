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
/* BINDTOOL_HEADER_FILE(probe_density_b.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c3d3b58d1bbd93eb7985604da20e4d66)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/probe_density_b.h>
// pydoc.h is automatically generated in the build directory
#include <probe_density_b_pydoc.h>

void bind_probe_density_b(py::module& m)
{

    using probe_density_b = ::gr::digital::probe_density_b;


    py::class_<probe_density_b,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<probe_density_b>>(m, "probe_density_b", D(probe_density_b))

        .def(py::init(&probe_density_b::make), py::arg("alpha"), D(probe_density_b, make))


        .def("density", &probe_density_b::density, D(probe_density_b, density))


        .def("set_alpha",
             &probe_density_b::set_alpha,
             py::arg("alpha"),
             D(probe_density_b, set_alpha))

        ;
}
