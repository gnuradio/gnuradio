/* -*- c++ -*- */
/*
 * Copyright 2020-2021 Free Software Foundation, Inc.
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
/* BINDTOOL_HEADER_FILE(block.h)                                                   */
/* BINDTOOL_HEADER_FILE_HASH(6326cce2f22e30ab332ef966343f4a50)                     */
/***********************************************************************************/

#include "soapy_common.h"

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/soapy/block.h>
// pydoc.h is automatically generated in the build directory
#include <block_pydoc.h>

#include <algorithm>
#include <cassert>

// Assumption: we've validated that this key exists,
// probably through get_setting_info.
static gr::soapy::arginfo_t
get_specific_arginfo(const gr::soapy::arginfo_list_t& arginfo_list,
                     const std::string& key)
{
    auto iter = std::find_if(
        arginfo_list.begin(),
        arginfo_list.end(),
        [&key](const gr::soapy::arginfo_t& arginfo) { return (arginfo.key == key); });
    assert(iter != arginfo_list.end());

    return (*iter);
}

void bind_block(py::module& m)
{

    using block = ::gr::soapy::block;


    py::class_<block, gr::block, gr::basic_block, std::shared_ptr<block>>(
        m, "block", D(block))


        .def("set_frontend_mapping",
             &block::set_frontend_mapping,
             py::arg("frontend_mapping"),
             D(block, set_frontend_mapping))


        .def("get_frontend_mapping",
             &block::get_frontend_mapping,
             D(block, get_frontend_mapping))


        .def("get_channel_info",
             &block::get_channel_info,
             py::arg("channel"),
             D(block, get_channel_info))


        .def("set_sample_rate",
             &block::set_sample_rate,
             py::arg("channel"),
             py::arg("sample_rate"),
             D(block, set_sample_rate))


        .def("get_sample_rate",
             &block::get_sample_rate,
             py::arg("channel"),
             D(block, get_sample_rate))


        .def("get_sample_rate_range",
             &block::get_sample_rate_range,
             py::arg("channel"),
             D(block, get_sample_rate_range))


        .def("set_frequency",
             (void(block::*)(size_t, double)) & block::set_frequency,
             py::arg("channel"),
             py::arg("freq"),
             D(block, set_frequency, 0))


        .def("set_frequency",
             (void(block::*)(size_t, const std::string&, double)) & block::set_frequency,
             py::arg("channel"),
             py::arg("name"),
             py::arg("freq"),
             D(block, set_frequency, 1))


        .def("get_frequency",
             (double(block::*)(size_t) const) & block::get_frequency,
             py::arg("channel"),
             D(block, get_frequency, 0))


        .def("get_frequency",
             (double(block::*)(size_t, const std::string&) const) & block::get_frequency,
             py::arg("channel"),
             py::arg("name"),
             D(block, get_frequency, 1))


        .def("list_frequencies",
             &block::list_frequencies,
             py::arg("channel"),
             D(block, list_frequencies))


        .def("get_frequency_range",
             (gr::soapy::range_list_t(block::*)(size_t) const) &
                 block::get_frequency_range,
             py::arg("channel"),
             D(block, get_frequency_range, 0))


        .def("get_frequency_range",
             (gr::soapy::range_list_t(block::*)(size_t, const std::string&) const) &
                 block::get_frequency_range,
             py::arg("channel"),
             py::arg("name"),
             D(block, get_frequency_range, 1))


        .def("get_frequency_args_info",
             &block::get_frequency_args_info,
             py::arg("channel"),
             D(block, get_frequency_args_info))


        .def("set_bandwidth",
             &block::set_bandwidth,
             py::arg("channel"),
             py::arg("bandwidth"),
             D(block, set_bandwidth))


        .def("get_bandwidth",
             &block::get_bandwidth,
             py::arg("channel"),
             D(block, get_bandwidth))


        .def("get_bandwidth_range",
             &block::get_bandwidth_range,
             py::arg("channel"),
             D(block, get_bandwidth_range))


        .def("list_antennas",
             &block::list_antennas,
             py::arg("channel"),
             D(block, list_antennas))


        .def("set_antenna",
             &block::set_antenna,
             py::arg("channel"),
             py::arg("name"),
             D(block, set_antenna))


        .def(
            "get_antenna", &block::get_antenna, py::arg("channel"), D(block, get_antenna))


        .def("has_gain_mode",
             &block::has_gain_mode,
             py::arg("channel"),
             D(block, has_gain_mode))


        .def("set_gain_mode",
             &block::set_gain_mode,
             py::arg("channel"),
             py::arg("automatic"),
             D(block, set_gain_mode))


        .def("get_gain_mode",
             &block::get_gain_mode,
             py::arg("channel"),
             D(block, get_gain_mode))


        .def("list_gains", &block::list_gains, py::arg("channel"), D(block, list_gains))


        .def("set_gain",
             (void(block::*)(size_t, double)) & block::set_gain,
             py::arg("channel"),
             py::arg("gain"),
             D(block, set_gain, 0))


        .def("set_gain",
             (void(block::*)(size_t, const std::string&, double)) & block::set_gain,
             py::arg("channel"),
             py::arg("name"),
             py::arg("gain"),
             D(block, set_gain, 1))


        .def("get_gain",
             (double(block::*)(size_t) const) & block::get_gain,
             py::arg("channel"),
             D(block, get_gain, 0))


        .def("get_gain",
             (double(block::*)(size_t, const std::string&) const) & block::get_gain,
             py::arg("channel"),
             py::arg("name"),
             D(block, get_gain, 1))


        .def("get_gain_range",
             (gr::soapy::range_t(block::*)(size_t) const) & block::get_gain_range,
             py::arg("channel"),
             D(block, get_gain_range, 0))


        .def("get_gain_range",
             (gr::soapy::range_t(block::*)(size_t, const std::string&) const) &
                 block::get_gain_range,
             py::arg("channel"),
             py::arg("name"),
             D(block, get_gain_range, 1))


        .def("has_frequency_correction",
             &block::has_frequency_correction,
             py::arg("channel"),
             D(block, has_frequency_correction))


        .def("set_frequency_correction",
             &block::set_frequency_correction,
             py::arg("channel"),
             py::arg("freq_correction"),
             D(block, set_frequency_correction))


        .def("get_frequency_correction",
             &block::get_frequency_correction,
             py::arg("channel"),
             D(block, get_frequency_correction))


        .def("has_dc_offset_mode",
             &block::has_dc_offset_mode,
             py::arg("channel"),
             D(block, has_dc_offset_mode))


        .def("set_dc_offset_mode",
             &block::set_dc_offset_mode,
             py::arg("channel"),
             py::arg("automatic"),
             D(block, set_dc_offset_mode))


        .def("get_dc_offset_mode",
             &block::get_dc_offset_mode,
             py::arg("channel"),
             D(block, get_dc_offset_mode))


        .def("has_dc_offset",
             &block::has_dc_offset,
             py::arg("channel"),
             D(block, has_dc_offset))


        .def("set_dc_offset",
             &block::set_dc_offset,
             py::arg("channel"),
             py::arg("dc_offset"),
             D(block, set_dc_offset))


        .def("get_dc_offset",
             &block::get_dc_offset,
             py::arg("channel"),
             D(block, get_dc_offset))


        .def("has_iq_balance",
             &block::has_iq_balance,
             py::arg("channel"),
             D(block, has_iq_balance))


        .def("set_iq_balance",
             &block::set_iq_balance,
             py::arg("channel"),
             py::arg("iq_balance"),
             D(block, set_iq_balance))


        .def("get_iq_balance",
             &block::get_iq_balance,
             py::arg("channel"),
             D(block, get_iq_balance))


        .def("has_iq_balance_mode",
             &block::has_iq_balance_mode,
             py::arg("channel"),
             D(block, has_iq_balance_mode))


        .def("set_iq_balance_mode",
             &block::set_iq_balance_mode,
             py::arg("channel"),
             py::arg("automatic"),
             D(block, set_iq_balance_mode))


        .def("get_iq_balance_mode",
             &block::get_iq_balance_mode,
             py::arg("channel"),
             D(block, get_iq_balance_mode))


        .def("set_master_clock_rate",
             &block::set_master_clock_rate,
             py::arg("clock_rate"),
             D(block, set_master_clock_rate))


        .def("get_master_clock_rate",
             &block::get_master_clock_rate,
             D(block, get_master_clock_rate))


        .def("get_master_clock_rates",
             &block::get_master_clock_rates,
             D(block, get_master_clock_rate))


        .def("set_reference_clock_rate",
             &block::set_reference_clock_rate,
             py::arg("clock_rate"),
             D(block, set_reference_clock_rate))


        .def("get_reference_clock_rate",
             &block::get_reference_clock_rate,
             D(block, get_reference_clock_rate))


        .def("get_reference_clock_rates",
             &block::get_reference_clock_rates,
             D(block, get_reference_clock_rate))


        .def("list_clock_sources",
             &block::list_clock_sources,
             D(block, list_clock_sources))


        .def("set_clock_source",
             &block::set_clock_source,
             py::arg("clock_source"),
             D(block, set_clock_source))


        .def("get_clock_source", &block::get_clock_source, D(block, get_clock_source))


        .def("list_time_sources", &block::list_time_sources, D(block, list_time_sources))


        .def("set_time_source",
             &block::set_time_source,
             py::arg("time_source"),
             D(block, set_time_source))


        .def("get_time_source", &block::get_time_source, D(block, get_time_source))


        .def("has_hardware_time",
             &block::has_hardware_time,
             py::arg("what") = "",
             D(block, has_hardware_time))


        .def("set_hardware_time",
             &block::set_hardware_time,
             py::arg("what") = "",
             py::arg("time_ns"),
             D(block, set_hardware_time))


        .def("get_hardware_time",
             &block::get_hardware_time,
             py::arg("what") = "",
             D(block, get_hardware_time))


        .def("list_sensors",
             (std::vector<std::string>(block::*)() const) & block::list_sensors,
             D(block, list_sensors, 0))

        .def("get_sensor_info",
             (gr::soapy::arginfo_t(block::*)(const std::string&) const) &
                 block::get_sensor_info,
             py::arg("key"),
             D(block, get_sensor_info, 0))

        .def(
            "read_sensor",
            [](const block& self, const std::string& key) -> py::object {
                const auto arginfo = self.get_sensor_info(key);

                return cast_string_to_arginfo_type(arginfo.type, arginfo.value);
            },
            py::arg("key"),
            D(block, read_sensor, 0))

        .def("list_sensors",
             (std::vector<std::string>(block::*)(size_t) const) & block::list_sensors,
             py::arg("channel"),
             D(block, list_sensors, 1))

        .def("get_sensor_info",
             (gr::soapy::arginfo_t(block::*)(size_t, const std::string&) const) &
                 block::get_sensor_info,
             py::arg("channel"),
             py::arg("key"),
             D(block, get_sensor_info, 1))

        .def(
            "read_sensor",
            [](const block& self, size_t channel, const std::string& key) -> py::object {
                const auto arginfo = self.get_sensor_info(channel, key);

                return cast_string_to_arginfo_type(arginfo.type, arginfo.value);
            },
            py::arg("channel"),
            py::arg("key"),
            D(block, read_sensor, 1))


        .def("list_register_interfaces",
             &block::list_register_interfaces,
             D(block, list_register_interfaces))


        .def("write_register",
             &block::write_register,
             py::arg("name"),
             py::arg("addr"),
             py::arg("value"),
             D(block, write_register))


        .def("read_register",
             &block::read_register,
             py::arg("name"),
             py::arg("addr"),
             D(block, read_register))


        .def("write_registers",
             &block::write_registers,
             py::arg("name"),
             py::arg("addr"),
             py::arg("value"),
             D(block, write_registers))


        .def("read_registers",
             &block::read_registers,
             py::arg("name"),
             py::arg("addr"),
             py::arg("length"),
             D(block, read_registers))

        .def("get_setting_info",
             (gr::soapy::arginfo_list_t(block::*)() const) & block::get_setting_info,
             D(block, get_setting_info, 0))

        .def(
            "write_setting",
            [](block& self, const std::string& key, py::object value) -> void {
                auto setting_info = cast_pyobject_to_arginfo_string(value);

                self.write_setting(key, setting_info.value);
            },
            py::arg("key"),
            py::arg("value"),
            D(block, write_setting, 0))

        .def(
            "read_setting",
            [](const block& self, const std::string& key) -> py::object {
                const auto setting_info =
                    get_specific_arginfo(self.get_setting_info(), key);

                return cast_string_to_arginfo_type(setting_info.type, setting_info.value);
            },
            py::arg("key"),
            D(block, read_setting, 0))

        .def("get_setting_info",
             (gr::soapy::arginfo_list_t(block::*)(size_t) const) &
                 block::get_setting_info,
             py::arg("channel"),
             D(block, get_setting_info, 0))

        .def(
            "write_setting",
            [](block& self, size_t channel, const std::string& key, py::object value)
                -> void {
                auto setting_info = cast_pyobject_to_arginfo_string(value);

                self.write_setting(channel, key, setting_info.value);
            },
            py::arg("channel"),
            py::arg("key"),
            py::arg("value"),
            D(block, write_setting, 0))

        .def(
            "read_setting",
            [](const block& self, size_t channel, const std::string& key) -> py::object {
                const auto setting_info =
                    get_specific_arginfo(self.get_setting_info(channel), key);

                return cast_string_to_arginfo_type(setting_info.type, setting_info.value);
            },
            py::arg("channel"),
            py::arg("key"),
            D(block, read_setting, 0))


        .def("list_gpio_banks", &block::list_gpio_banks, D(block, list_gpio_banks))


        .def("write_gpio",
             (void(block::*)(const std::string&, unsigned)) & block::write_gpio,
             py::arg("bank"),
             py::arg("value"),
             D(block, write_gpio, 0))


        .def("write_gpio",
             (void(block::*)(const std::string&, unsigned, unsigned)) & block::write_gpio,
             py::arg("bank"),
             py::arg("value"),
             py::arg("mask"),
             D(block, write_gpio, 1))


        .def("read_gpio", &block::read_gpio, py::arg("bank"), D(block, read_gpio))


        .def("write_gpio_dir",
             (void(block::*)(const std::string&, unsigned)) & block::write_gpio_dir,
             py::arg("bank"),
             py::arg("value"),
             D(block, write_gpio_dir, 0))


        .def("write_gpio_dir",
             (void(block::*)(const std::string&, unsigned, unsigned)) &
                 block::write_gpio_dir,
             py::arg("bank"),
             py::arg("value"),
             py::arg("mask"),
             D(block, write_gpio_dir, 1))


        .def("read_gpio_dir",
             &block::read_gpio_dir,
             py::arg("bank"),
             D(block, read_gpio_dir))


        .def("write_i2c",
             &block::write_i2c,
             py::arg("addr"),
             py::arg("data"),
             D(block, write_i2c))


        .def("read_i2c",
             &block::read_i2c,
             py::arg("addr"),
             py::arg("length"),
             D(block, read_i2c))


        .def("transact_spi",
             &block::transact_spi,
             py::arg("addr"),
             py::arg("data"),
             py::arg("num_bits"),
             D(block, transact_spi))


        .def("list_uarts", &block::list_uarts, D(block, list_uarts))


        .def("write_uart",
             &block::write_uart,
             py::arg("which"),
             py::arg("data"),
             D(block, write_uart))


        .def("read_uart",
             &block::read_uart,
             py::arg("which"),
             py::arg("timeout_us") = 100000,
             D(block, read_uart))

        ;
}
