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
/* BINDTOOL_HEADER_FILE(attr_updater.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(e33efa383a9b7d46a5615e6166cefd1c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/iio/attr_updater.h>
// pydoc.h is automatically generated in the build directory
#include <attr_updater_pydoc.h>

void bind_attr_updater(py::module& m)
{

    using attr_updater = gr::iio::attr_updater;


    py::class_<attr_updater, gr::block, gr::basic_block, std::shared_ptr<attr_updater>>(
        m, "attr_updater", D(attr_updater))

        .def(py::init(&attr_updater::make),
             py::arg("attribute"),
             py::arg("value"),
             py::arg("interval_ms"),
             D(attr_updater, make))

        .def("set_value",
             &attr_updater::set_value,
             py::arg("value"),
             D(attr_updater, make))

        ;
}
