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
/* BINDTOOL_HEADER_FILE(selective_fading_model.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a70748fab2290742010437e061b6c7db)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/channels/selective_fading_model.h>
// pydoc.h is automatically generated in the build directory
#include <selective_fading_model_pydoc.h>

void bind_selective_fading_model(py::module& m)
{

    using selective_fading_model = ::gr::channels::selective_fading_model;


    py::class_<selective_fading_model,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<selective_fading_model>>(
        m, "selective_fading_model", D(selective_fading_model))

        .def(py::init(&selective_fading_model::make),
             py::arg("N"),
             py::arg("fDTs"),
             py::arg("LOS"),
             py::arg("K"),
             py::arg("seed"),
             py::arg("delays"),
             py::arg("mags"),
             py::arg("ntaps"),
             D(selective_fading_model, make))


        .def("fDTs", &selective_fading_model::fDTs, D(selective_fading_model, fDTs))


        .def("K", &selective_fading_model::K, D(selective_fading_model, K))


        .def("step", &selective_fading_model::step, D(selective_fading_model, step))


        .def("set_fDTs",
             &selective_fading_model::set_fDTs,
             py::arg("fDTs"),
             D(selective_fading_model, set_fDTs))


        .def("set_K",
             &selective_fading_model::set_K,
             py::arg("K"),
             D(selective_fading_model, set_K))


        .def("set_step",
             &selective_fading_model::set_step,
             py::arg("step"),
             D(selective_fading_model, set_step))

        ;
}
