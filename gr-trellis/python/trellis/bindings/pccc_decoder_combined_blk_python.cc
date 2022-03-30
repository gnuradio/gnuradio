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
/* BINDTOOL_HEADER_FILE(pccc_decoder_combined_blk.h) */
/* BINDTOOL_HEADER_FILE_HASH(ee939cb6c38f26044996af9e6657a80a)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/trellis/pccc_decoder_combined_blk.h>
// pydoc.h is automatically generated in the build directory
#include <pccc_decoder_combined_blk_pydoc.h>

template <class IN_T, class OUT_T>
void bind_pccc_decoder_combined_blk_template(py::module& m, const char* classname)
{
    using pccc_decoder_combined_blk = gr::trellis::pccc_decoder_combined_blk<IN_T, OUT_T>;

    py::class_<pccc_decoder_combined_blk,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pccc_decoder_combined_blk>>(m, classname)
        .def(py::init(&gr::trellis::pccc_decoder_combined_blk<IN_T, OUT_T>::make),
             py::arg("FSMo"),
             py::arg("STo0"),
             py::arg("SToK"),
             py::arg("FSMi"),
             py::arg("STi0"),
             py::arg("STiK"),
             py::arg("INTERLEAVER"),
             py::arg("blocklength"),
             py::arg("repetitions"),
             py::arg("SISO_TYPE"),
             py::arg("D"),
             py::arg("TABLE"),
             py::arg("METRIC_TYPE"),
             py::arg("scaling"))
        .def("FSM1", &pccc_decoder_combined_blk::FSM1)
        .def("ST10", &pccc_decoder_combined_blk::ST10)
        .def("ST1K", &pccc_decoder_combined_blk::ST1K)
        .def("FSM2", &pccc_decoder_combined_blk::FSM2)
        .def("ST20", &pccc_decoder_combined_blk::ST20)
        .def("ST2K", &pccc_decoder_combined_blk::ST2K)
        .def("INTERLEAVER", &pccc_decoder_combined_blk::INTERLEAVER)
        .def("blocklength", &pccc_decoder_combined_blk::blocklength)
        .def("repetitions", &pccc_decoder_combined_blk::repetitions)
        .def("SISO_TYPE", &pccc_decoder_combined_blk::SISO_TYPE)
        .def("D", &pccc_decoder_combined_blk::D)
        .def("TABLE", &pccc_decoder_combined_blk::TABLE)
        .def("METRIC_TYPE", &pccc_decoder_combined_blk::METRIC_TYPE)
        .def("scaling", &pccc_decoder_combined_blk::scaling);
}

void bind_pccc_decoder_combined_blk(py::module& m)
{
    bind_pccc_decoder_combined_blk_template<float, std::uint8_t>(
        m, "pccc_decoder_combined_fb");
    bind_pccc_decoder_combined_blk_template<float, std::int16_t>(
        m, "pccc_decoder_combined_fs");
    bind_pccc_decoder_combined_blk_template<float, std::int32_t>(
        m, "pccc_decoder_combined_fi");
    bind_pccc_decoder_combined_blk_template<gr_complex, std::uint8_t>(
        m, "pccc_decoder_combined_cb");
    bind_pccc_decoder_combined_blk_template<gr_complex, std::int16_t>(
        m, "pccc_decoder_combined_cs");
    bind_pccc_decoder_combined_blk_template<gr_complex, std::int32_t>(
        m, "pccc_decoder_combined_ci");
}
