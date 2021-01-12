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
/* BINDTOOL_HEADER_FILE(tag_checker.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(e602a721975a2c2c15dbbb419eb57c15)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/tag_checker.h>
// pydoc.h is automatically generated in the build directory
#include <tag_checker_pydoc.h>

void bind_tag_checker(py::module& m)
{

    using tag_checker = ::gr::tag_checker;


    py::class_<tag_checker, std::shared_ptr<tag_checker>>(
        m, "tag_checker", D(tag_checker))

        .def(py::init<std::vector<gr::tag_t, std::allocator<gr::tag_t>>&>(),
             py::arg("tags"),
             D(tag_checker, tag_checker, 0))
        .def(py::init<gr::tag_checker const&>(),
             py::arg("arg0"),
             D(tag_checker, tag_checker, 1))


        .def("get_tags",
             &tag_checker::get_tags,
             py::arg("tag_list"),
             py::arg("offset"),
             D(tag_checker, get_tags))

        ;


    py::module m_messages = m.def_submodule("messages");
}
