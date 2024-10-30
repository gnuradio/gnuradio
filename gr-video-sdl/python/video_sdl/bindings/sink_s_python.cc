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
/* BINDTOOL_HEADER_FILE(sink_s.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(6b999bf6a9f589af5ec7193a0f0cbca4)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/video_sdl/sink_s.h>
// pydoc.h is automatically generated in the build directory
#include <sink_s_pydoc.h>

void bind_sink_s(py::module& m)
{

    using sink_s = ::gr::video_sdl::sink_s;


    py::class_<sink_s,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<sink_s>>(m, "sink_s", D(sink_s))

        .def(py::init(&sink_s::make),
             py::arg("framerate"),
             py::arg("width"),
             py::arg("height"),
             py::arg("dst_width"),
             py::arg("dst_height"),
             D(sink_s, make))


        ;
}
