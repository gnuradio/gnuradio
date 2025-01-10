/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "block_gateway.h"

#include <pybind11/pybind11.h>

#include <pybind11/complex.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <memory>
#include <optional>
#include <string>

namespace py = pybind11;

block_gateway::block_gateway(const py::handle& py_handle,
                             const std::string& name,
                             gr::io_signature::sptr in_sig,
                             gr::io_signature::sptr out_sig)
    : gr::block(name, in_sig, out_sig), d_py_handle(py_handle)
{
}

block_gateway::sptr block_gateway::make(const py::object& py_handle,
                                        const std::string& name,
                                        gr::io_signature::sptr in_sig,
                                        gr::io_signature::sptr out_sig)
{
    return std::make_shared<block_gateway>(py_handle, name, in_sig, out_sig);
}

void block_gateway::_add_item_tag(unsigned int which_output, const gr::tag_t& tag)
{
    return gr::block::add_item_tag(which_output, tag);
}

void block_gateway::_add_item_tag(unsigned int which_output,
                                  uint64_t abs_offset,
                                  const pmt::pmt_t& key,
                                  const pmt::pmt_t& value,
                                  const pmt::pmt_t& srcid)
{
    return gr::block::add_item_tag(which_output, abs_offset, key, value, srcid);
}

std::vector<gr::tag_t> block_gateway::_get_tags_in_range(unsigned int which_input,
                                                         uint64_t abs_start,
                                                         uint64_t abs_end)
{
    std::vector<gr::tag_t> tags;
    gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end);
    return tags;
}

std::vector<gr::tag_t> block_gateway::_get_tags_in_range(unsigned int which_input,
                                                         uint64_t abs_start,
                                                         uint64_t abs_end,
                                                         const pmt::pmt_t& key)
{
    std::vector<gr::tag_t> tags;
    gr::block::get_tags_in_range(tags, which_input, abs_start, abs_end, key);
    return tags;
}

std::vector<gr::tag_t> block_gateway::_get_tags_in_window(unsigned int which_input,
                                                          uint64_t rel_start,
                                                          uint64_t rel_end)
{
    std::vector<gr::tag_t> tags;
    gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end);
    return tags;
}

std::vector<gr::tag_t> block_gateway::_get_tags_in_window(unsigned int which_input,
                                                          uint64_t rel_start,
                                                          uint64_t rel_end,
                                                          const pmt::pmt_t& key)
{
    std::vector<gr::tag_t> tags;
    gr::block::get_tags_in_window(tags, which_input, rel_start, rel_end, key);
    return tags;
}

std::optional<gr::tag_t> block_gateway::_get_first_tag_in_range(unsigned int which_input,
                                                                uint64_t start,
                                                                uint64_t end,
                                                                const pmt::pmt_t& key)
{
    return gr::block::get_first_tag_in_range(which_input, start, end, key);
}
std::optional<gr::tag_t>
block_gateway::_get_first_tag_in_range(unsigned int which_input,
                                       uint64_t start,
                                       uint64_t end,
                                       std::function<bool(const gr::tag_t&)> predicate)
{
    return gr::block::get_first_tag_in_range(which_input, start, end, predicate);
}

void block_gateway::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    py::gil_scoped_acquire acquire;

    py::object ret_ninput_items_required =
        d_py_handle.attr("handle_forecast")(noutput_items, ninput_items_required.size());
    ninput_items_required = ret_ninput_items_required.cast<std::vector<int>>();
}

int block_gateway::general_work(int noutput_items,
                                gr_vector_int& ninput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("handle_general_work")(
        noutput_items, ninput_items, input_items, output_items);

    return ret.cast<int>();
}

bool block_gateway::start()
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("start")();
    return ret.cast<bool>();
}

bool block_gateway::stop()
{
    py::gil_scoped_acquire acquire;

    py::object ret = d_py_handle.attr("stop")();
    return ret.cast<bool>();
}

void block_gateway::set_msg_handler_pybind(const pmt::pmt_t& which_port,
                                           std::string& handler_name)
{
    if (msg_queue.find(which_port) == msg_queue.end()) {
        throw std::runtime_error(
            "attempt to set_msg_handler_pybind() on invalid input message port!");
    }
    d_msg_handlers_pybind[which_port] = handler_name;
}

bool block_gateway::has_msg_handler(pmt::pmt_t which_port)
{
    if (d_msg_handlers_pybind.find(which_port) != d_msg_handlers_pybind.end()) {
        return true;
    }
    return gr::block::has_msg_handler(which_port);
}

void block_gateway::dispatch_msg(pmt::pmt_t which_port, pmt::pmt_t msg)
{
    // Is there a handler?
    if (d_msg_handlers_pybind.find(which_port) != d_msg_handlers_pybind.end()) {
        // d_msg_handlers_pybind[which_port]->calleval(msg); // Yes, invoke it.
        py::gil_scoped_acquire acquire;
        // std::string handler_name(d_msg_handlers_pybind[which_port]);
        py::object ret = d_py_handle.attr(d_msg_handlers_pybind[which_port].c_str())(msg);
    } else {
        // Pass to generic dispatcher if not found
        gr::block::dispatch_msg(which_port, msg);
    }
}

void bind_block_gateway(py::module& m)
{
    py::class_<block_gateway, gr::block, gr::basic_block, std::shared_ptr<block_gateway>>(
        m, "block_gateway")

        .def(py::init(&block_gateway::make),
             py::arg("p"),
             py::arg("name"),
             py::arg("in_sig"),
             py::arg("out_sig"))

        .def("add_item_tag",
             (void(block_gateway::*)(unsigned int, const gr::tag_t&)) &
                 block_gateway::_add_item_tag,
             py::arg("which_output"),
             py::arg("tag"))

        .def("add_item_tag",
             (void(block_gateway::*)(unsigned int,
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
                block_gateway::_get_tags_in_window,
            py::arg("which_input"),
            py::arg("rel_start"),
            py::arg("rel_end"))

        .def("get_tags_in_window",
             (std::vector<gr::tag_t>(block_gateway::*)(
                 unsigned int, uint64_t, uint64_t, const pmt::pmt_t&)) &
                 block_gateway::_get_tags_in_window,
             py::arg("which_input"),
             py::arg("rel_start"),
             py::arg("rel_end"),
             py::arg("key"))

        .def("set_msg_handler_pybind",
             &block_gateway::set_msg_handler_pybind,
             py::arg("which_port"),
             py::arg("handler_name"))

        .def_property_readonly("logger", &block_gateway::_get_logger)

        .def("get_first_tag_in_range",
             py::overload_cast<unsigned, uint64_t, uint64_t, const pmt::pmt_t&>(
                 &block_gateway::_get_first_tag_in_range))
        .def("get_first_tag_in_range",
             py::overload_cast<unsigned,
                               uint64_t,
                               uint64_t,
                               std::function<bool(const gr::tag_t&)>>(
                 &block_gateway::_get_first_tag_in_range));


    ;
    py::enum_<gw_block_t>(m, "gw_block_t")
        .value("GW_BLOCK_GENERAL", GW_BLOCK_GENERAL)
        .value("GW_BLOCK_SYNC", GW_BLOCK_SYNC)
        .value("GW_BLOCK_DECIM", GW_BLOCK_DECIM)
        .value("GW_BLOCK_INTERP", GW_BLOCK_INTERP)
        .export_values();
}
