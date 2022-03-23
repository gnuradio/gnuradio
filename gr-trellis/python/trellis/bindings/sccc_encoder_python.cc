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
/* BINDTOOL_HEADER_FILE(sccc_encoder.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(1f6cf0ef063243ab9b0187805a6f9cde)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/trellis/sccc_encoder.h>
// pydoc.h is automatically generated in the build directory
#include <sccc_encoder_pydoc.h>

template <class IN_T, class OUT_T>
void bind_sccc_encoder_template(py::module& m, const char* classname)
{
    using sccc_encoder = gr::trellis::sccc_encoder<IN_T, OUT_T>;

    py::class_<sccc_encoder,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<sccc_encoder>>(m, classname)
        .def(py::init(&gr::trellis::sccc_encoder<IN_T, OUT_T>::make),
             py::arg("FSMo"),
             py::arg("STo"),
             py::arg("FSMi"),
             py::arg("STi"),
             py::arg("INTERLEAVER"),
             py::arg("blocklength") = 0)
        .def("FSMo", &sccc_encoder::FSMo)
        .def("STo", &sccc_encoder::STo)
        .def("FSMi", &sccc_encoder::FSMi)
        .def("STi", &sccc_encoder::STi)
        .def("INTERLEAVER", &sccc_encoder::INTERLEAVER)
        .def("blocklength", &sccc_encoder::blocklength);
}

void bind_sccc_encoder(py::module& m)
{
    bind_sccc_encoder_template<std::uint8_t, std::uint8_t>(m, "sccc_encoder_bb");
    bind_sccc_encoder_template<std::uint8_t, std::int16_t>(m, "sccc_encoder_bs");
    bind_sccc_encoder_template<std::uint8_t, std::int32_t>(m, "sccc_encoder_bi");
    bind_sccc_encoder_template<std::int16_t, std::int16_t>(m, "sccc_encoder_ss");
    bind_sccc_encoder_template<std::int16_t, std::int32_t>(m, "sccc_encoder_si");
    bind_sccc_encoder_template<std::int32_t, std::int32_t>(m, "sccc_encoder_ii");
}


