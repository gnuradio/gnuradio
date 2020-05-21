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
/* BINDTOOL_HEADER_FILE(interp_fir_filter.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(02d8c5636e8ab7234432daa29114a75c)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/filter/interp_fir_filter.h>

template <class IN_T, class OUT_T, class TAP_T>
void bind_interp_fir_filter_template(py::module& m, const char* classname)
{
    using interp_fir_filter = gr::filter::interp_fir_filter<IN_T, OUT_T, TAP_T>;

    py::class_<interp_fir_filter,
               gr::sync_interpolator,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<interp_fir_filter>>(m, classname)
        .def(py::init(&gr::filter::interp_fir_filter<IN_T, OUT_T, TAP_T>::make),
             py::arg("interpolation"),
             py::arg("taps"))

        .def("set_taps", &interp_fir_filter::set_taps, py::arg("taps"))
        .def("taps", &interp_fir_filter::taps);
}

void bind_interp_fir_filter(py::module& m)
{
    bind_interp_fir_filter_template<gr_complex, gr_complex, gr_complex>(
        m, "interp_fir_filter_ccc");
    bind_interp_fir_filter_template<gr_complex, gr_complex, float>(
        m, "interp_fir_filter_ccf");
    bind_interp_fir_filter_template<float, gr_complex, gr_complex>(
        m, "interp_fir_filter_fcc");
    bind_interp_fir_filter_template<float, float, float>(m, "interp_fir_filter_fff");
    bind_interp_fir_filter_template<float, std::int16_t, float>(m,
                                                                "interp_fir_filter_fsf");
    bind_interp_fir_filter_template<std::int16_t, gr_complex, gr_complex>(
        m, "interp_fir_filter_scc");
}
