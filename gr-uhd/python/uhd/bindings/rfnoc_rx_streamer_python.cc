/*
 * Copyright 2021 Marcus Müller
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
/* BINDTOOL_HEADER_FILE(rfnoc_rx_streamer.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(c9627fa218093e7f23303f35d8fe709c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/uhd/rfnoc_rx_streamer.h>
// pydoc.h is automatically generated in the build directory
#include <rfnoc_rx_streamer_pydoc.h>

void bind_rfnoc_rx_streamer(py::module& m)
{

    using rfnoc_rx_streamer = ::gr::uhd::rfnoc_rx_streamer;


    py::class_<rfnoc_rx_streamer,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<rfnoc_rx_streamer>>(
        m, "rfnoc_rx_streamer", D(rfnoc_rx_streamer))

        .def(py::init(&rfnoc_rx_streamer::make),
             py::arg("graph"),
             py::arg("num_chans"),
             py::arg("stream_args"),
             py::arg("vlen") = 1,
             py::arg("issue_stream_cmd_on_start") = true,
             D(rfnoc_rx_streamer, make))


        .def("get_unique_id",
             &rfnoc_rx_streamer::get_unique_id,
             D(rfnoc_rx_streamer, get_unique_id))

        ;
}
