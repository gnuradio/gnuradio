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
/* BINDTOOL_HEADER_FILE(ofdm_frame_equalizer_vcvc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(8b3b47c9bf16426f21bbc0338c27f0ee)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/ofdm_frame_equalizer_vcvc.h>
// pydoc.h is automatically generated in the build directory
#include <ofdm_frame_equalizer_vcvc_pydoc.h>

void bind_ofdm_frame_equalizer_vcvc(py::module& m)
{

    using ofdm_frame_equalizer_vcvc = ::gr::digital::ofdm_frame_equalizer_vcvc;


    py::class_<ofdm_frame_equalizer_vcvc,
               gr::tagged_stream_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<ofdm_frame_equalizer_vcvc>>(
        m, "ofdm_frame_equalizer_vcvc", D(ofdm_frame_equalizer_vcvc))

        .def(py::init(&ofdm_frame_equalizer_vcvc::make),
             py::arg("equalizer"),
             py::arg("cp_len"),
             py::arg("tsb_key") = "frame_len",
             py::arg("propagate_channel_state") = false,
             py::arg("fixed_frame_len") = 0,
             D(ofdm_frame_equalizer_vcvc, make))


        ;
}
