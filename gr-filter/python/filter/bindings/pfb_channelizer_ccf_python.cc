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
/* BINDTOOL_HEADER_FILE(pfb_channelizer_ccf.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(58e2ad3c8033d6b70a93e1210389251a)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/pfb_channelizer_ccf.h>
// pydoc.h is automatically generated in the build directory
#include <pfb_channelizer_ccf_pydoc.h>

void bind_pfb_channelizer_ccf(py::module& m)
{

    using pfb_channelizer_ccf = ::gr::filter::pfb_channelizer_ccf;


    py::class_<pfb_channelizer_ccf,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pfb_channelizer_ccf>>(
        m, "pfb_channelizer_ccf", D(pfb_channelizer_ccf))

        .def(py::init(&pfb_channelizer_ccf::make),
             py::arg("numchans"),
             py::arg("taps"),
             py::arg("oversample_rate"),
             D(pfb_channelizer_ccf, make))


        .def("set_taps",
             &pfb_channelizer_ccf::set_taps,
             py::arg("taps"),
             D(pfb_channelizer_ccf, set_taps))


        .def("print_taps",
             &pfb_channelizer_ccf::print_taps,
             D(pfb_channelizer_ccf, print_taps))


        .def("taps", &pfb_channelizer_ccf::taps, D(pfb_channelizer_ccf, taps))


        .def("set_channel_map",
             &pfb_channelizer_ccf::set_channel_map,
             py::arg("map"),
             D(pfb_channelizer_ccf, set_channel_map))


        .def("channel_map",
             &pfb_channelizer_ccf::channel_map,
             D(pfb_channelizer_ccf, channel_map))

        ;
}
