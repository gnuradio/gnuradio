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
/* BINDTOOL_HEADER_FILE(dvbt_convolutional_deinterleaver.h)                        */
/* BINDTOOL_HEADER_FILE_HASH(a92209c39bfe0affdf32e6547c368eaf)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/dtv/dvbt_convolutional_deinterleaver.h>
// pydoc.h is automatically generated in the build directory
#include <dvbt_convolutional_deinterleaver_pydoc.h>

void bind_dvbt_convolutional_deinterleaver(py::module& m)
{

    using dvbt_convolutional_deinterleaver = ::gr::dtv::dvbt_convolutional_deinterleaver;


    py::class_<dvbt_convolutional_deinterleaver,
               gr::block,
               gr::basic_block,
               std::shared_ptr<dvbt_convolutional_deinterleaver>>(
        m, "dvbt_convolutional_deinterleaver", D(dvbt_convolutional_deinterleaver))

        .def(py::init(&dvbt_convolutional_deinterleaver::make),
             py::arg("nsize"),
             py::arg("I"),
             py::arg("M"),
             D(dvbt_convolutional_deinterleaver, make))


        ;
}
