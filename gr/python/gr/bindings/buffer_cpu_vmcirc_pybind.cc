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

#include <gnuradio/buffer_cpu_vmcirc.h>
// pydoc.h is automatically generated in the build directory
// #include <edge_pydoc.h>

void bind_vmcircbuf(py::module& m)
{
    using buffer_cpu_vmcirc_properties = ::gr::buffer_cpu_vmcirc_properties;

    py::enum_<gr::buffer_cpu_vmcirc_type>(m, "buffer_cpu_vmcirc_type")
        .value("AUTO", gr::buffer_cpu_vmcirc_type::AUTO)
        .value("SYSV_SHM", gr::buffer_cpu_vmcirc_type::SYSV_SHM)
        .value("MMAP_SHM", gr::buffer_cpu_vmcirc_type::MMAP_SHM)
        .value("MMAP_TMPFILE", gr::buffer_cpu_vmcirc_type::MMAP_TMPFILE)
        .export_values();


    py::class_<buffer_cpu_vmcirc_properties,
               gr::buffer_properties,
               std::shared_ptr<buffer_cpu_vmcirc_properties>>(
        m, "buffer_cpu_vmcirc_properties")
        .def_static("make",
                    &buffer_cpu_vmcirc_properties::make,
                    py::arg("buffer_type") = gr::buffer_cpu_vmcirc_type::AUTO);
}
