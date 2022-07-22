/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

#include <gnuradio/python_block.h>
// PYBIND11_MAKE_OPAQUE(std::vector<gr::block_work_output>);
// PYBIND11_MAKE_OPAQUE(std::vector<uint8_t>);
void bind_python_block(py::module& m)
{
    using python_block = gr::python_block;
    py::class_<python_block, gr::block, gr::node, std::shared_ptr<python_block>>(
        m, "python_block")

        .def(py::init(&python_block::make), py::arg("p"), py::arg("name"));

    using python_sync_block = gr::python_sync_block;
    py::class_<python_sync_block,
               gr::sync_block,
               gr::node,
               std::shared_ptr<python_sync_block>>(m, "python_sync_block")

        .def(py::init(&python_sync_block::make), py::arg("p"), py::arg("name"));

    py::enum_<gr::py_block_t>(m, "py_block_t")
        .value("PY_BLOCK_GENERAL", gr::PY_BLOCK_GENERAL)
        .value("PY_BLOCK_SYNC", gr::PY_BLOCK_SYNC)
        .value("PY_BLOCK_DECIM", gr::PY_BLOCK_DECIM)
        .value("PY_BLOCK_INTERP", gr::PY_BLOCK_INTERP)
        .export_values();
}
