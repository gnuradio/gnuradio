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
/* BINDTOOL_HEADER_FILE(repeat.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(11d545f53d36223bc0f70ba8e207696b)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/blocks/repeat.h>
// pydoc.h is automatically generated in the build directory
#include <repeat_pydoc.h>

void bind_repeat(py::module& m)
{

    using repeat = ::gr::blocks::repeat;


    py::class_<repeat, gr::block, gr::basic_block, std::shared_ptr<repeat>>(
        m, "repeat", D(repeat))

        .def(py::init(&repeat::make),
             py::arg("itemsize"),
             py::arg("repeat"),
             D(repeat, make))


        .def("interpolation", &repeat::interpolation, D(repeat, interpolation))


        .def("set_interpolation",
             &repeat::set_interpolation,
             py::arg("interp"),
             D(repeat, set_interpolation))

        ;
}
