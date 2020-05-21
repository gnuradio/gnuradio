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
/* BINDTOOL_HEADER_FILE(map_bb.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(9cd6dc85b2557412507a371eed58c285)                     */ 
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/digital/map_bb.h>
// pydoc.h is automatically generated in the build directory
#include <map_bb_pydoc.h>

void bind_map_bb(py::module& m)
{

    using map_bb = ::gr::digital::map_bb;


    py::class_<map_bb,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<map_bb>>(m, "map_bb", D(map_bb))

        .def(py::init(&map_bb::make), py::arg("map"), D(map_bb, make))


        .def("set_map", &map_bb::set_map, py::arg("map"), D(map_bb, set_map))


        .def("map", &map_bb::map, D(map_bb, map))

        ;
}
