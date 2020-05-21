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
/* BINDTOOL_HEADER_FILE(ofdm_chanest_vcvc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(82caca03752b79f24d3d33c573cf570b)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/ofdm_chanest_vcvc.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_chanest_vcvc_pydoc.h>

void bind_ofdm_chanest_vcvc(py::module& m)
{

    using ofdm_chanest_vcvc = ::gr::digital::ofdm_chanest_vcvc;


    py::class_<ofdm_chanest_vcvc,
               gr::block,
               gr::basic_block,
               std::shared_ptr<ofdm_chanest_vcvc>>(
        m, "ofdm_chanest_vcvc", D(ofdm_chanest_vcvc))

        .def(py::init(&ofdm_chanest_vcvc::make),
             py::arg("sync_symbol1"),
             py::arg("sync_symbol2"),
             py::arg("n_data_symbols"),
             py::arg("eq_noise_red_len") = 0,
             py::arg("max_carr_offset") = -1,
             py::arg("force_one_sync_symbol") = false,
             D(ofdm_chanest_vcvc, make))


        ;
}
