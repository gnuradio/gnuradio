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
/* BINDTOOL_HEADER_FILE(rpccallbackregister_base.h) */
/* BINDTOOL_HEADER_FILE_HASH(59490b02954c38c1ac4959ee0cd05bd3)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/rpccallbackregister_base.h>


void bind_rpccallbackregister_base(py::module& m)
{
    py::module m_messages = m.def_submodule("messages");
    m.attr("DISPNULL") = DISPNULL;
    m.attr("DISPTIME") = DISPTIME;
    m.attr("DISPXY") = DISPXY;
    m.attr("DISPPSD") = DISPPSD;
    m.attr("DISPSPEC") = DISPSPEC;
    m.attr("DISPRAST") = DISPRAST;


    m.attr("DISPOPTCPLX") = DISPOPTCPLX;
    m.attr("DISPOPTLOG") = DISPOPTLOG;
    m.attr("DISPOPTSTEM") = DISPOPTSTEM;
    m.attr("DISPOPTSTRIP") = DISPOPTSTRIP;
    m.attr("DISPOPTSCATTER") = DISPOPTSCATTER;

    py::enum_<priv_lvl_t>(m, "priv_lvl_t")
        .value("RPC_PRIVLVL_ALL", RPC_PRIVLVL_ALL)   // 0
        .value("RPC_PRIVLVL_MIN", RPC_PRIVLVL_MIN)   // 9
        .value("RPC_PRIVLVL_NONE", RPC_PRIVLVL_NONE) // 10
        .export_values();

    py::enum_<KnobType>(m, "KnobType")
        .value("KNOBBOOL", KNOBBOOL)
        .value("KNOBCHAR", KNOBCHAR)
        .value("KNOBINT", KNOBINT)
        .value("KNOBFLOAT", KNOBFLOAT)
        .value("KNOBDOUBLE", KNOBDOUBLE)
        .value("KNOBSTRING", KNOBSTRING)
        .value("KNOBLONG", KNOBLONG)
        .value("KNOBVECBOOL", KNOBVECBOOL)
        .value("KNOBCOMPLEX", KNOBCOMPLEX)
        .value("KNOBCOMPLEXD", KNOBCOMPLEXD)
        .value("KNOBVECCHAR", KNOBVECCHAR)
        .value("KNOBVECINT", KNOBVECINT)
        .value("KNOBVECFLOAT", KNOBVECFLOAT)
        .value("KNOBVECDOUBLE", KNOBVECDOUBLE)
        .value("KNOBVECSTRING", KNOBVECSTRING)
        .value("KNOBVECLONG", KNOBVECLONG)
        .export_values();
}
