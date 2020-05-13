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
/* BINDTOOL_HEADER_FILE(pack_k_bits_bb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(36978280edb60b63713b9eef499f3873)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/pack_k_bits_bb.h>
// pydoc.h is automatically generated in the build directory
#include <pack_k_bits_bb_pydoc.h>

void bind_pack_k_bits_bb(py::module& m)
{

    using pack_k_bits_bb = ::gr::blocks::pack_k_bits_bb;


    py::class_<pack_k_bits_bb,
               gr::sync_decimator,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<pack_k_bits_bb>>(m, "pack_k_bits_bb", D(pack_k_bits_bb))

        .def(py::init(&pack_k_bits_bb::make), py::arg("k"), D(pack_k_bits_bb, make))


        ;
}
