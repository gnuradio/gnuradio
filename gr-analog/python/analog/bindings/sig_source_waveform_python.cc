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
/* BINDTOOL_HEADER_FILE(sig_source_waveform.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(77b052d5e9bc93e2cf8b63a5014e2132)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/analog/sig_source_waveform.h>

void bind_sig_source_waveform(py::module& m)
{

    py::enum_<gr::analog::gr_waveform_t>(m, "gr_waveform_t")
        .value("GR_CONST_WAVE", gr::analog::GR_CONST_WAVE) // 100
        .value("GR_SIN_WAVE", gr::analog::GR_SIN_WAVE)     // 101
        .value("GR_COS_WAVE", gr::analog::GR_COS_WAVE)     // 102
        .value("GR_SQR_WAVE", gr::analog::GR_SQR_WAVE)     // 103
        .value("GR_TRI_WAVE", gr::analog::GR_TRI_WAVE)     // 104
        .value("GR_SAW_WAVE", gr::analog::GR_SAW_WAVE)     // 105
        .export_values();

    py::implicitly_convertible<int, gr::analog::gr_waveform_t>();
}
