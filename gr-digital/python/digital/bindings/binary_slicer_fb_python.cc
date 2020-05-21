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
/* BINDTOOL_HEADER_FILE(binary_slicer_fb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(a35245abbcda72cccfd32e1008a3c818)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/binary_slicer_fb.h>
// pydoc.h is automatically generated in the build directory
#include <binary_slicer_fb_pydoc.h>

void bind_binary_slicer_fb(py::module& m)
{

    using binary_slicer_fb = ::gr::digital::binary_slicer_fb;


    py::class_<binary_slicer_fb,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<binary_slicer_fb>>(
        m, "binary_slicer_fb", D(binary_slicer_fb))

        .def(py::init(&binary_slicer_fb::make), D(binary_slicer_fb, make))


        ;
}
