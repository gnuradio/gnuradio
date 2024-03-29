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
/* BINDTOOL_HEADER_FILE(catv_config.h)                                             */
/* BINDTOOL_HEADER_FILE_HASH(fe49dc21788df88d41abc50779439db5)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/dtv/catv_config.h>
// pydoc.h is automatically generated in the build directory
#include <catv_config_pydoc.h>

void bind_catv_config(py::module& m)
{


    py::enum_<::gr::dtv::catv_constellation_t>(m, "catv_constellation_t")
        .value("CATV_MOD_64QAM", ::gr::dtv::CATV_MOD_64QAM)   // 0
        .value("CATV_MOD_256QAM", ::gr::dtv::CATV_MOD_256QAM) // 1
        .export_values();

    py::implicitly_convertible<int, gr::dtv::catv_constellation_t>();
}
