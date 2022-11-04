/*
 * Copyright 2022 Free Software Foundation, Inc.
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
/* BINDTOOL_USE_PYGCCXML(1)                                                        */
/* BINDTOOL_HEADER_FILE(sink_c.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(be3c6d3500abbebbfdc610cb7a951715)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/qtgui/sink_c.h>
// pydoc.h is automatically generated in the build directory
#include <sink_c_pydoc.h>

void bind_sink_c(py::module& m)
{

    using sink_c = ::gr::qtgui::sink_c;


    py::class_<sink_c, gr::block, gr::basic_block, std::shared_ptr<sink_c>>(
        m, "sink_c", D(sink_c))

        .def(py::init(&sink_c::make),
             py::arg("fftsize"),
             py::arg("wintype"),
             py::arg("fc"),
             py::arg("bw"),
             py::arg("name"),
             py::arg("plotfreq"),
             py::arg("plotwaterfall"),
             py::arg("plottime"),
             py::arg("plotconst"),
             py::arg("parent") = nullptr,
             D(sink_c, make))


        .def("exec_", &sink_c::exec_, D(sink_c, exec_))


        .def(
            "qwidget",
            [](sink_c& self) { return reinterpret_cast<uintptr_t>(self.qwidget()); },
            D(sink_c, qwidget))


        .def("set_fft_size",
             &sink_c::set_fft_size,
             py::arg("fftsize"),
             D(sink_c, set_fft_size))


        .def("fft_size", &sink_c::fft_size, D(sink_c, fft_size))


        .def("set_frequency_range",
             &sink_c::set_frequency_range,
             py::arg("centerfreq"),
             py::arg("bandwidth"),
             D(sink_c, set_frequency_range))


        .def("set_fft_power_db",
             &sink_c::set_fft_power_db,
             py::arg("min"),
             py::arg("max"),
             D(sink_c, set_fft_power_db))


        .def("enable_rf_freq",
             &sink_c::enable_rf_freq,
             py::arg("en"),
             D(sink_c, enable_rf_freq))


        .def("set_update_time",
             &sink_c::set_update_time,
             py::arg("t"),
             D(sink_c, set_update_time))

        ;
}
