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
/* BINDTOOL_HEADER_FILE(blockinterleaver_xx.h)                                     */
/* BINDTOOL_HEADER_FILE_HASH(85e1f0a639e25ead5b2b9ddc44d51196)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/blockinterleaver_xx.h>

template <typename T>
void bind_blockinterleaver_template(py::module& m, const char* classname)
{
    using blockinterleaver = gr::blocks::blockinterleaver_xx<T>;

    py::class_<blockinterleaver,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<blockinterleaver>>(m, classname)
        .def(py::init(&gr::blocks::blockinterleaver_xx<T>::make),
             py::arg("interleaver_indices"),
             py::arg("interleaver_mode"),
             py::arg("is_packed") = false)
        .def("interleaver_indices", &blockinterleaver::interleaver_indices)
        .def("deinterleaver_indices", &blockinterleaver::deinterleaver_indices)
        .def("is_packed", &blockinterleaver::is_packed)
        .def("interleaver_mode", &blockinterleaver::interleaver_mode);
}

void bind_blockinterleaver_xx(py::module& m)
{
    bind_blockinterleaver_template<uint8_t>(m, "blockinterleaver_bb");
    bind_blockinterleaver_template<gr_complex>(m, "blockinterleaver_cc");
    bind_blockinterleaver_template<float>(m, "blockinterleaver_ff");
    bind_blockinterleaver_template<int32_t>(m, "blockinterleaver_ii");
    bind_blockinterleaver_template<int16_t>(m, "blockinterleaver_ss");
}
