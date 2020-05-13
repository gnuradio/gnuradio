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
/* BINDTOOL_HEADER_FILE(xor_blk.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(9695bb06d84235a7cfcc67e7ada1f052)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/xor_blk.h>

template <typename T>
void bind_xor_blk_template(py::module& m, const char* classname)
{
    using xor_blk = gr::blocks::xor_blk<T>;

    py::class_<xor_blk,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<xor_blk>>(m, classname)
        .def(py::init(&gr::blocks::xor_blk<T>::make), py::arg("vlen") = 1);
}

void bind_xor_blk(py::module& m)
{
    bind_xor_blk_template<std::uint8_t>(m, "xor_bb");
    bind_xor_blk_template<std::int16_t>(m, "xor_ss");
    bind_xor_blk_template<std::int32_t>(m, "xor_ii");
}
