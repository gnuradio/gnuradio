
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

#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <gnuradio/types.h>
#include <numpy/arrayobject.h>

namespace py = pybind11;

#include <gnuradio/schedulers/nbt/scheduler_nbt.h>

// We need this hack because import_array() returns NULL
// for newer Python versions.
// This function is also necessary because it ensures access to the C API
// and removes a warning.
void* init_numpy()
{
    import_array();
    return NULL;
}

PYBIND11_MODULE(scheduler_nbt_python, m)
{
    // Initialize the numpy C API
    // (otherwise we will see segmentation faults)
    init_numpy();

    // Allow access to base block methods
    py::module::import("gnuradio.gr");

    using nbt = gr::schedulers::scheduler_nbt;
    py::class_<nbt, gr::scheduler, std::shared_ptr<nbt>>(m, "scheduler_nbt")
        .def(py::init(&gr::schedulers::scheduler_nbt::make),
             py::arg("name") = "multi_threaded",
             py::arg("fixed_buf_size") = 32768)
        .def("add_block_group",
             &gr::schedulers::scheduler_nbt::add_block_group,
             py::arg("blocks"),
             py::arg("name") = "",
             py::arg("affinity_mask") = std::vector<unsigned int>{});
}
