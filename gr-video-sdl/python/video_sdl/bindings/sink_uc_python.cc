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
/* BINDTOOL_HEADER_FILE(sink_uc.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(49c2aee2770a14d64b019ea2a551e7c7)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/video_sdl/sink_uc.h>
// pydoc.h is automatically generated in the build directory
#include <sink_uc_pydoc.h>

void bind_sink_uc(py::module& m)
{

    using sink_uc = ::gr::video_sdl::sink_uc;


    py::class_<sink_uc,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<sink_uc>>(m, "sink_uc", D(sink_uc))

        .def(py::init(&sink_uc::make),
             py::arg("framerate"),
             py::arg("width"),
             py::arg("height"),
             py::arg("format"),
             py::arg("dst_width"),
             py::arg("dst_height"),
             D(sink_uc, make))


        ;
}
