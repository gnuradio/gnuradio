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

#include <gnuradio/block_gateway.h>

void bind_block_gateway(py::module& m)
{
    using block_gateway = gr::block_gateway;
    py::class_<block_gateway, gr::block, gr::basic_block, std::shared_ptr<block_gateway>>(
        m, "block_gateway")

        .def(py::init(&block_gateway::make),
             py::arg("p"),
             py::arg("name"),
             py::arg("in_sig"),
             py::arg("out_sig"))

        .def("add_item_tag",
             (void (block_gateway::*)(unsigned int, const gr::tag_t&)) &
                 block_gateway::_add_item_tag,
             py::arg("which_output"),
             py::arg("tag"))

        .def("add_item_tag",
             (void (block_gateway::*)(unsigned int,
                                      uint64_t,
                                      const pmt::pmt_t&,
                                      const pmt::pmt_t&,
                                      const pmt::pmt_t&)) &
                 block_gateway::_add_item_tag,
             py::arg("which_output"),
             py::arg("abs_offset"),
             py::arg("key"),
             py::arg("value"),
             py::arg("srcid") = pmt::PMT_F)


        .def(
            "get_tags_in_range",
            (std::vector<gr::tag_t>(block_gateway::*)(unsigned int, uint64_t, uint64_t)) &
                block_gateway::_get_tags_in_range,
            py::arg("which_input"),
            py::arg("abs_start"),
            py::arg("abs_end"))

        .def("get_tags_in_range",
             (std::vector<gr::tag_t>(block_gateway::*)(
                 unsigned int, uint64_t, uint64_t, const pmt::pmt_t&)) &
                 block_gateway::_get_tags_in_range,
             py::arg("which_input"),
             py::arg("abs_start"),
             py::arg("abs_end"),
             py::arg("key"))

        .def(
            "get_tags_in_window",
            (std::vector<gr::tag_t>(block_gateway::*)(unsigned int, uint64_t, uint64_t)) &
                block_gateway::_get_tags_in_range,
            py::arg("which_input"),
            py::arg("rel_start"),
            py::arg("rel_end"))

        .def("get_tags_in_window",
             (std::vector<gr::tag_t>(block_gateway::*)(
                 unsigned int, uint64_t, uint64_t, const pmt::pmt_t&)) &
                 block_gateway::_get_tags_in_range,
             py::arg("which_input"),
             py::arg("rel_start"),
             py::arg("rel_end"),
             py::arg("key"))

        .def("set_msg_handler_pybind",
             &block_gateway::set_msg_handler_pybind,
             py::arg("which_port"),
             py::arg("handler_name"))

        ;
    py::enum_<gr::gw_block_t>(m, "gw_block_t")
        .value("GW_BLOCK_GENERAL", gr::GW_BLOCK_GENERAL)
        .value("GW_BLOCK_SYNC", gr::GW_BLOCK_SYNC)
        .value("GW_BLOCK_DECIM", gr::GW_BLOCK_DECIM)
        .value("GW_BLOCK_INTERP", gr::GW_BLOCK_INTERP)
        .export_values();
}
