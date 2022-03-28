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
/* BINDTOOL_HEADER_FILE(crc32_async_bb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(21a4e7d0804db99d0f75ddbe2fe9ef51)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/crc32_async_bb.h>
// pydoc.h is automatically generated in the build directory
#include <crc32_async_bb_pydoc.h>

void bind_crc32_async_bb(py::module& m)
{

    using crc32_async_bb = ::gr::digital::crc32_async_bb;


    py::class_<crc32_async_bb,
               gr::block,
               gr::basic_block,
               std::shared_ptr<crc32_async_bb>>(m, "crc32_async_bb", D(crc32_async_bb))

        .def(py::init(&crc32_async_bb::make),
             py::arg("check") = false,
             D(crc32_async_bb, make))


        ;
}
