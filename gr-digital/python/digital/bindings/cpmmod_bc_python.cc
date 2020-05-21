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
/* BINDTOOL_HEADER_FILE(cpmmod_bc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(fd001012f591feffe43c60f710e918bf)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/cpmmod_bc.h>
// pydoc.h is automatically generated in the build directory
#include <cpmmod_bc_pydoc.h>

void bind_cpmmod_bc(py::module& m)
{

    using cpmmod_bc = ::gr::digital::cpmmod_bc;


    py::class_<cpmmod_bc, gr::hier_block2, gr::basic_block, std::shared_ptr<cpmmod_bc>>(
        m, "cpmmod_bc", D(cpmmod_bc))

        .def(py::init(&cpmmod_bc::make),
             py::arg("type"),
             py::arg("h"),
             py::arg("samples_per_sym"),
             py::arg("L"),
             py::arg("beta") = 0.3,
             D(cpmmod_bc, make))


        .def_static("make_gmskmod_bc",
                    &cpmmod_bc::make_gmskmod_bc,
                    py::arg("samples_per_sym") = 2,
                    py::arg("L") = 4,
                    py::arg("beta") = 0.3,
                    D(cpmmod_bc, make_gmskmod_bc))


        .def("taps", &cpmmod_bc::taps, D(cpmmod_bc, taps))


        .def("type", &cpmmod_bc::type, D(cpmmod_bc, type))


        .def("index", &cpmmod_bc::index, D(cpmmod_bc, index))


        .def(
            "samples_per_sym", &cpmmod_bc::samples_per_sym, D(cpmmod_bc, samples_per_sym))


        .def("beta", &cpmmod_bc::beta, D(cpmmod_bc, beta))

        ;
}
