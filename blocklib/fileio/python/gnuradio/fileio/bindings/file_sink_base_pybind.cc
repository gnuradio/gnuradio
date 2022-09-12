/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/fileio/file_sink_base.h>
// pydoc.h is automatically generated in the build directory
// #include <block_pydoc.h>

void bind_file_sink_base(py::module& m)
{
    using file_sink_base = ::gr::fileio::file_sink_base;

    py::class_<file_sink_base, std::shared_ptr<file_sink_base>>(m, "file_sink_base")
        .def("set_unbuffered", &file_sink_base::set_unbuffered, py::arg("unbuffered"));
}
