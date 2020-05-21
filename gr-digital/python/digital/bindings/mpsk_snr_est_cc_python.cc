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
/* BINDTOOL_HEADER_FILE(mpsk_snr_est_cc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(d62f385f709f14dc1fea43f73c9a7a47)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/mpsk_snr_est_cc.h>
// pydoc.h is automatically generated in the build directory
#include <mpsk_snr_est_cc_pydoc.h>

void bind_mpsk_snr_est_cc(py::module& m)
{

    using mpsk_snr_est_cc = ::gr::digital::mpsk_snr_est_cc;


    py::class_<mpsk_snr_est_cc,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<mpsk_snr_est_cc>>(m, "mpsk_snr_est_cc", D(mpsk_snr_est_cc))

        .def(py::init(&mpsk_snr_est_cc::make),
             py::arg("type"),
             py::arg("tag_nsamples") = 10000,
             py::arg("alpha") = 0.001,
             D(mpsk_snr_est_cc, make))


        .def("snr", &mpsk_snr_est_cc::snr, D(mpsk_snr_est_cc, snr))


        .def("type", &mpsk_snr_est_cc::type, D(mpsk_snr_est_cc, type))


        .def(
            "tag_nsample", &mpsk_snr_est_cc::tag_nsample, D(mpsk_snr_est_cc, tag_nsample))


        .def("alpha", &mpsk_snr_est_cc::alpha, D(mpsk_snr_est_cc, alpha))


        .def("set_type",
             &mpsk_snr_est_cc::set_type,
             py::arg("t"),
             D(mpsk_snr_est_cc, set_type))


        .def("set_tag_nsample",
             &mpsk_snr_est_cc::set_tag_nsample,
             py::arg("n"),
             D(mpsk_snr_est_cc, set_tag_nsample))


        .def("set_alpha",
             &mpsk_snr_est_cc::set_alpha,
             py::arg("alpha"),
             D(mpsk_snr_est_cc, set_alpha))

        ;
}
