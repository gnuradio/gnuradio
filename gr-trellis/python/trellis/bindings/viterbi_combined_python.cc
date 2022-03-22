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
/* BINDTOOL_HEADER_FILE(viterbi_combined.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(42e9e8a3daa4ef03ff9d7110e4c37a62)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/trellis/viterbi_combined.h>
// pydoc.h is automatically generated in the build directory
#include <viterbi_combined_pydoc.h>

template<typename IN_T, typename OUT_T>
void bind_viterbi_combined_template(py::module &m, const char *classname) {
    using viterbi_combined = gr::trellis::viterbi_combined<IN_T, OUT_T>;

    py::class_<viterbi_combined,
               gr::block,
               gr::basic_block,
               std::shared_ptr <viterbi_combined>> (m, classname)
        .def(py::init(&gr::trellis::viterbi_combined<IN_T, OUT_T>::make),
             py::arg("FSM"),
             py::arg("K"),
             py::arg("S0"),
             py::arg("SK"),
             py::arg("D"),
             py::arg("TABLE"),
             py::arg("TYPE"))
        .def("FSM", &viterbi_combined::FSM)
        .def("K", &viterbi_combined::K)
        .def("S0", &viterbi_combined::S0)
        .def("SK", &viterbi_combined::SK)
        .def("D", &viterbi_combined::D)
        .def("set_FSM", &viterbi_combined::set_FSM)
        .def("set_K", &viterbi_combined::set_K)
        .def("set_S0", &viterbi_combined::set_S0)
        .def("set_SK", &viterbi_combined::set_SK)
        .def("set_D", &viterbi_combined::set_D)
        .def("set_TABLE", &viterbi_combined::set_TABLE)
        .def("set_TYPE", &viterbi_combined::set_TYPE);
}

void bind_viterbi_combined(py::module &m)
{
    bind_viterbi_combined_template<std::int16_t, std::uint8_t>(m, "viterbi_combined_sb");
    bind_viterbi_combined_template<std::int16_t, std::int16_t>(m, "viterbi_combined_ss");
    bind_viterbi_combined_template<std::int16_t, std::int32_t>(m, "viterbi_combined_si");
    bind_viterbi_combined_template<std::int32_t, std::uint8_t>(m, "viterbi_combined_ib");
    bind_viterbi_combined_template<std::int32_t, std::int16_t>(m, "viterbi_combined_is");
    bind_viterbi_combined_template<std::int32_t, std::int32_t>(m, "viterbi_combined_ii");
    bind_viterbi_combined_template<float, std::uint8_t>(m, "viterbi_combined_fb");
    bind_viterbi_combined_template<float, std::int16_t>(m, "viterbi_combined_fs");
    bind_viterbi_combined_template<float, std::int32_t>(m, "viterbi_combined_fi");
    bind_viterbi_combined_template<gr_complex, std::uint8_t>(m, "viterbi_combined_cb");
    bind_viterbi_combined_template<gr_complex, std::int16_t>(m, "viterbi_combined_cs");
    bind_viterbi_combined_template<gr_complex, std::int32_t>(m, "viterbi_combined_ci");
}
