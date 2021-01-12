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
/* BINDTOOL_HEADER_FILE(probe_signal.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(ea7457920b50e2aa80be0802cc74c757)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/probe_signal.h>

template <typename T>
void bind_probe_signal_template(py::module& m, const char* classname)
{
    using probe_signal = gr::blocks::probe_signal<T>;

    py::class_<probe_signal,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<probe_signal>>(m, classname)
        .def(py::init(&gr::blocks::probe_signal<T>::make))
        .def("level", &probe_signal::level);
}

void bind_probe_signal(py::module& m)
{
    bind_probe_signal_template<std::uint8_t>(m, "probe_signal_b");
    bind_probe_signal_template<std::int16_t>(m, "probe_signal_s");
    bind_probe_signal_template<std::int32_t>(m, "probe_signal_i");
    bind_probe_signal_template<float>(m, "probe_signal_f");
    bind_probe_signal_template<gr_complex>(m, "probe_signal_c");
}
