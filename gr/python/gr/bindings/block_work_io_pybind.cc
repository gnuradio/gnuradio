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

#include <gnuradio/block_work_io.h>

void bind_block_work_io(py::module& m)
{
    py::enum_<gr::work_return_code_t>(m, "work_return_t")
        .value("WORK_ERROR", gr::work_return_code_t::WORK_ERROR) // -100
        .value("WORK_INSUFFICIENT_OUTPUT_ITEMS",
               gr::work_return_code_t::WORK_INSUFFICIENT_OUTPUT_ITEMS) // -3
        .value("WORK_INSUFFICIENT_INPUT_ITEMS",
               gr::work_return_code_t::WORK_INSUFFICIENT_INPUT_ITEMS) // -2
        .value("WORK_DONE", gr::work_return_code_t::WORK_DONE)        // -1
        .value("WORK_OK", gr::work_return_code_t::WORK_OK)            //  0
        .export_values();

    py::class_<gr::block_work_output>(m, "block_work_output")
        .def_readwrite("n_items", &gr::block_work_output::n_items)
        .def("buffer", &gr::block_work_output::buf)
        .def_readwrite("n_produced", &gr::block_work_output::n_produced)

        .def("raw_items", &gr::block_work_output::raw_items)
        .def("produce", &gr::block_work_output::produce);

    py::class_<gr::block_work_input>(m, "block_work_input")
        .def_readwrite("n_items", &gr::block_work_input::n_items)
        .def("buffer", &gr::block_work_input::buf)
        .def_readwrite("n_consumed", &gr::block_work_input::n_consumed)

        .def("nitems_read", &gr::block_work_input::nitems_read)
        .def("raw_items", &gr::block_work_input::raw_items)
        .def("consume", &gr::block_work_input::consume)
        .def("tags_in_window", &gr::block_work_input::tags_in_window);

    py::class_<gr::io_vec_wrap<gr::block_work_output>>(m, "io_vec_wrap_output")
        .def(
            "__getitem__",
            [](gr::io_vec_wrap<gr::block_work_output>& obj, size_t idx) {
                return &obj[idx];
            },
            py::return_value_policy::reference)
        .def(
            "__getitem__",
            [](gr::io_vec_wrap<gr::block_work_output>& obj, const std::string& name) {
                return &obj[name];
            },
            py::return_value_policy::reference);

    py::class_<gr::io_vec_wrap<gr::block_work_input>>(m, "io_vec_wrap_input")
        .def(
            "__getitem__",
            [](gr::io_vec_wrap<gr::block_work_input>& obj, size_t idx) {
                return &obj[idx];
            },
            py::return_value_policy::reference)
        .def(
            "__getitem__",
            [](gr::io_vec_wrap<gr::block_work_input>& obj, const std::string& name) {
                return &obj[name];
            },
            py::return_value_policy::reference);

    py::class_<gr::work_io, std::unique_ptr<gr::work_io>>(m, "work_io")
        .def("inputs", &gr::work_io::inputs, py::return_value_policy::reference)
        .def("outputs", &gr::work_io::outputs, py::return_value_policy::reference)
        .def("consume_each", &gr::work_io::consume_each)
        .def("produce_each", &gr::work_io::produce_each)
        .def("min_noutput_items", &gr::work_io::min_noutput_items)
        .def("min_ninput_items", &gr::work_io::min_ninput_items);
}
