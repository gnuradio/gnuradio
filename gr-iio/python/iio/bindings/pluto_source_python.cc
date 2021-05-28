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
/* BINDTOOL_HEADER_FILE(pluto_source.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(6b5fd53d9435ff6ec687d86045729de3)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/iio/pluto_source.h>
// pydoc.h is automatically generated in the build directory
#include <pluto_source_pydoc.h>

void bind_pluto_source(py::module& m)
{

    using pluto_source = gr::iio::pluto_source;

    py::class_<pluto_source,
               gr::hier_block2,
               gr::basic_block,
               std::shared_ptr<pluto_source>>(m, "pluto_source", D(pluto_source))

        .def(py::init(&pluto_source::make),
             py::arg("uri"),
             py::arg("buffer_size"),
             D(pluto_source, make))

        .def("set_frequency", &pluto_source::set_frequency, py::arg("longfrequency"))
        .def("set_samplerate", &pluto_source::set_samplerate, py::arg("samplerate"))
        .def("set_gain_mode", &pluto_source::set_gain_mode, py::arg("mode"))
        .def("set_gain", &pluto_source::set_gain, py::arg("gain_value"))
        .def("set_quadrature", &pluto_source::set_quadrature, py::arg("quadrature"))
        .def("set_rfdc", &pluto_source::set_rfdc, py::arg("rfdc"))
        .def("set_bbdc", &pluto_source::set_bbdc, py::arg("bbdc"))
        .def("set_filter_params", &pluto_source::set_filter_params)
        .def("set_len_tag_key", &pluto_source::set_len_tag_key, py::arg("len_tag_key"));
    ;
}
