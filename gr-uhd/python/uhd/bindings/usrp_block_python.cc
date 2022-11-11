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

#include <gnuradio/uhd/usrp_block.h>
// pydoc.h is automatically generated in the build directory
#include <usrp_block_pydoc.h>

void bind_usrp_block(py::module& m)
{

    using usrp_block = ::gr::uhd::usrp_block;

    m.attr("ALL_MBOARDS") = py::int_(::uhd::usrp::multi_usrp::ALL_MBOARDS);
    m.attr("ALL_LOS") = py::str(::uhd::usrp::multi_usrp::ALL_LOS);

    py::class_<usrp_block,
               gr::sync_block,
               gr::block,
               gr::basic_block,
               std::shared_ptr<usrp_block>>(m, "usrp_block", D(usrp_block))


        .def("set_subdev_spec",
             &usrp_block::set_subdev_spec,
             py::arg("spec"),
             py::arg("mboard") = 0,
             D(usrp_block, set_subdev_spec))


        .def("get_subdev_spec",
             &usrp_block::get_subdev_spec,
             py::arg("mboard") = 0,
             D(usrp_block, get_subdev_spec))


        .def("get_num_mboards",
             &usrp_block::get_num_mboards,
             D(usrp_block, get_num_mboards))


        .def("set_samp_rate",
             &usrp_block::set_samp_rate,
             py::arg("rate"),
             D(usrp_block, set_samp_rate))


        .def("get_samp_rate", &usrp_block::get_samp_rate, D(usrp_block, get_samp_rate))


        .def("get_samp_rates", &usrp_block::get_samp_rates, D(usrp_block, get_samp_rates))


        .def("set_center_freq",
             (uhd::tune_result_t(usrp_block::*)(uhd::tune_request_t const, size_t)) &
                 usrp_block::set_center_freq,
             py::arg("tune_request"),
             py::arg("chan") = 0,
             D(usrp_block, set_center_freq, 0))


        .def("set_center_freq",
             (uhd::tune_result_t(usrp_block::*)(double, size_t)) &
                 usrp_block::set_center_freq,
             py::arg("freq"),
             py::arg("chan") = 0,
             D(usrp_block, set_center_freq, 1))


        .def("get_center_freq",
             &usrp_block::get_center_freq,
             py::arg("chan") = 0,
             D(usrp_block, get_center_freq))


        .def("get_freq_range",
             &usrp_block::get_freq_range,
             py::arg("chan") = 0,
             D(usrp_block, get_freq_range))


        .def("set_gain",
             (void (usrp_block::*)(double, size_t, pmt::pmt_t)) & usrp_block::set_gain,
             py::arg("gain"),
             py::arg("chan") = 0,
             py::arg("direction") = pmt::PMT_NIL,
             D(usrp_block, set_gain, 0))


        .def("set_gain",
             (void (usrp_block::*)(double, std::string const&, size_t)) &
                 usrp_block::set_gain,
             py::arg("gain"),
             py::arg("name"),
             py::arg("chan") = 0,
             D(usrp_block, set_gain, 1))


        .def("set_normalized_gain",
             &usrp_block::set_normalized_gain,
             py::arg("norm_gain"),
             py::arg("chan") = 0,
             D(usrp_block, set_normalized_gain))


        .def("get_gain",
             (double (usrp_block::*)(size_t)) & usrp_block::get_gain,
             py::arg("chan") = 0,
             D(usrp_block, get_gain, 0))


        .def("get_gain",
             (double (usrp_block::*)(std::string const&, size_t)) & usrp_block::get_gain,
             py::arg("name"),
             py::arg("chan") = 0,
             D(usrp_block, get_gain, 1))


        .def("get_normalized_gain",
             &usrp_block::get_normalized_gain,
             py::arg("chan") = 0,
             D(usrp_block, get_normalized_gain))


        .def("get_gain_names",
             &usrp_block::get_gain_names,
             py::arg("chan") = 0,
             D(usrp_block, get_gain_names))


        .def("get_gain_range",
             (uhd::gain_range_t(usrp_block::*)(size_t)) & usrp_block::get_gain_range,
             py::arg("chan") = 0,
             D(usrp_block, get_gain_range, 0))


        .def("get_gain_range",
             (uhd::gain_range_t(usrp_block::*)(std::string const&, size_t)) &
                 usrp_block::get_gain_range,
             py::arg("name"),
             py::arg("chan") = 0,
             D(usrp_block, get_gain_range, 1))


        .def("has_power_reference",
             &usrp_block::has_power_reference,
             py::arg("chan") = 0,
             D(usrp_block, has_power_reference))


        .def("set_power_reference",
             &usrp_block::set_power_reference,
             py::arg("power_dbm"),
             py::arg("chan") = 0,
             D(usrp_block, set_power_reference))


        .def("get_power_reference",
             &usrp_block::get_power_reference,
             py::arg("chan") = 0,
             D(usrp_block, get_power_reference))


        .def("get_power_range",
             &usrp_block::get_power_range,
             py::arg("chan") = 0,
             D(usrp_block, get_power_range))


        .def("set_antenna",
             &usrp_block::set_antenna,
             py::arg("ant"),
             py::arg("chan") = 0,
             D(usrp_block, set_antenna))


        .def("get_antenna",
             &usrp_block::get_antenna,
             py::arg("chan") = 0,
             D(usrp_block, get_antenna))


        .def("get_antennas",
             &usrp_block::get_antennas,
             py::arg("chan") = 0,
             D(usrp_block, get_antennas))


        .def("set_bandwidth",
             &usrp_block::set_bandwidth,
             py::arg("bandwidth"),
             py::arg("chan") = 0,
             D(usrp_block, set_bandwidth))


        .def("get_bandwidth",
             &usrp_block::get_bandwidth,
             py::arg("chan") = 0,
             D(usrp_block, get_bandwidth))


        .def("get_bandwidth_range",
             &usrp_block::get_bandwidth_range,
             py::arg("chan") = 0,
             D(usrp_block, get_bandwidth_range))


        .def("get_sensor",
             &usrp_block::get_sensor,
             py::arg("name"),
             py::arg("chan") = 0,
             D(usrp_block, get_sensor))


        .def("get_sensor_names",
             &usrp_block::get_sensor_names,
             py::arg("chan") = 0,
             D(usrp_block, get_sensor_names))


        .def("get_dboard_sensor",
             &usrp_block::get_dboard_sensor,
             py::arg("name"),
             py::arg("chan") = 0,
             D(usrp_block, get_dboard_sensor))


        .def("get_dboard_sensor_names",
             &usrp_block::get_dboard_sensor_names,
             py::arg("chan") = 0,
             D(usrp_block, get_dboard_sensor_names))


        .def("get_mboard_sensor",
             &usrp_block::get_mboard_sensor,
             py::arg("name"),
             py::arg("mboard") = 0,
             D(usrp_block, get_mboard_sensor))


        .def("get_mboard_sensor_names",
             &usrp_block::get_mboard_sensor_names,
             py::arg("mboard") = 0,
             D(usrp_block, get_mboard_sensor_names))


        .def("get_time_source",
             &usrp_block::get_time_source,
             py::arg("mboard"),
             D(usrp_block, get_time_source))


        .def("get_time_sources",
             &usrp_block::get_time_sources,
             py::arg("mboard"),
             D(usrp_block, get_time_sources))


        .def("set_clock_source",
             &usrp_block::set_clock_source,
             py::arg("source"),
             py::arg("mboard") = 0,
             D(usrp_block, set_clock_source))


        .def("get_clock_source",
             &usrp_block::get_clock_source,
             py::arg("mboard"),
             D(usrp_block, get_clock_source))


        .def("get_clock_sources",
             &usrp_block::get_clock_sources,
             py::arg("mboard"),
             D(usrp_block, get_clock_sources))


        .def("get_clock_rate",
             &usrp_block::get_clock_rate,
             py::arg("mboard") = 0,
             D(usrp_block, get_clock_rate))


        .def("set_clock_rate",
             &usrp_block::set_clock_rate,
             py::arg("rate"),
             py::arg("mboard") = 0,
             D(usrp_block, set_clock_rate))


        .def("get_time_now",
             &usrp_block::get_time_now,
             py::arg("mboard") = 0,
             D(usrp_block, get_time_now))


        .def("get_time_last_pps",
             &usrp_block::get_time_last_pps,
             py::arg("mboard") = 0,
             D(usrp_block, get_time_last_pps))


        .def("set_time_now",
             &usrp_block::set_time_now,
             py::arg("time_spec"),
             py::arg("mboard") = 0,
             D(usrp_block, set_time_now))


        .def("set_time_next_pps",
             &usrp_block::set_time_next_pps,
             py::arg("time_spec"),
             D(usrp_block, set_time_next_pps))


        .def("set_time_unknown_pps",
             &usrp_block::set_time_unknown_pps,
             py::arg("time_spec"),
             D(usrp_block, set_time_unknown_pps))


        .def("set_command_time",
             &usrp_block::set_command_time,
             py::arg("time_spec"),
             py::arg("mboard") = 0,
             D(usrp_block, set_command_time))


        .def("clear_command_time",
             &usrp_block::clear_command_time,
             py::arg("mboard") = 0,
             D(usrp_block, clear_command_time))


        .def("get_dboard_iface",
             &usrp_block::get_dboard_iface,
             py::arg("chan") = 0,
             D(usrp_block, get_dboard_iface))


        .def("get_device", &usrp_block::get_device, D(usrp_block, get_device))


        .def("set_user_register",
             &usrp_block::set_user_register,
             py::arg("addr"),
             py::arg("data"),
             py::arg("mboard") = 0,
             D(usrp_block, set_user_register))


        .def("set_time_source",
             &usrp_block::set_time_source,
             py::arg("source"),
             py::arg("mboard") = 0,
             D(usrp_block, set_time_source))


        .def("set_stream_args",
             &usrp_block::set_stream_args,
             py::arg("stream_args"),
             D(usrp_block, set_stream_args))


        .def("get_gpio_banks",
             &usrp_block::get_gpio_banks,
             py::arg("mboard"),
             D(usrp_block, get_gpio_banks))


        .def("set_gpio_attr",
             &usrp_block::set_gpio_attr,
             py::arg("bank"),
             py::arg("attr"),
             py::arg("value"),
             py::arg("mask") = 4294967295U,
             py::arg("mboard") = 0,
             D(usrp_block, set_gpio_attr))


        .def("get_gpio_attr",
             &usrp_block::get_gpio_attr,
             py::arg("bank"),
             py::arg("attr"),
             py::arg("mboard") = 0,
             D(usrp_block, get_gpio_attr))


        .def("get_filter_names",
             &usrp_block::get_filter_names,
             py::arg("chan") = 0,
             D(usrp_block, get_filter_names))


        .def("set_filter",
             &usrp_block::set_filter,
             py::arg("path"),
             py::arg("filter"),
             py::arg("chan") = 0,
             D(usrp_block, set_filter))


        .def("get_filter",
             &usrp_block::get_filter,
             py::arg("path"),
             py::arg("chan") = 0,
             D(usrp_block, get_filter))

        .def(
            "get_usrp_info",
            [](usrp_block& self, const size_t chan = 0) {
                std::map<std::string, std::string> new_map;
                auto usrp_info = self.get_usrp_info(chan);
                for (const auto& k : self.get_usrp_info(chan).keys()) {
                    new_map[k] = usrp_info[k];
                }
                return new_map;
            },
            py::arg("chan") = 0,
            D(usrp_block, get_usrp_info));


    m.def("cmd_chan_key", &::gr::uhd::cmd_chan_key, D(cmd_chan_key));


    m.def("cmd_gain_key", &::gr::uhd::cmd_gain_key, D(cmd_gain_key));


    m.def("cmd_power_key", &::gr::uhd::cmd_power_key, D(cmd_power_key));


    m.def("cmd_freq_key", &::gr::uhd::cmd_freq_key, D(cmd_freq_key));


    m.def("cmd_lo_offset_key", &::gr::uhd::cmd_lo_offset_key, D(cmd_lo_offset_key));


    m.def("cmd_tune_key", &::gr::uhd::cmd_tune_key, D(cmd_tune_key));


    m.def("cmd_lo_freq_key", &::gr::uhd::cmd_lo_freq_key, D(cmd_lo_freq_key));


    m.def("cmd_dsp_freq_key", &::gr::uhd::cmd_dsp_freq_key, D(cmd_dsp_freq_key));


    m.def("cmd_rate_key", &::gr::uhd::cmd_rate_key, D(cmd_rate_key));


    m.def("cmd_bandwidth_key", &::gr::uhd::cmd_bandwidth_key, D(cmd_bandwidth_key));


    m.def("cmd_time_key", &::gr::uhd::cmd_time_key, D(cmd_time_key));


    m.def("cmd_mboard_key", &::gr::uhd::cmd_mboard_key, D(cmd_mboard_key));


    m.def("cmd_antenna_key", &::gr::uhd::cmd_antenna_key, D(cmd_antenna_key));


    m.def("cmd_direction_key", &::gr::uhd::cmd_direction_key, D(cmd_direction_key));


    m.def("cmd_tag_key", &::gr::uhd::cmd_tag_key, D(cmd_tag_key));


    m.def("direction_rx", &::gr::uhd::direction_rx, D(direction_rx));


    m.def("direction_tx", &::gr::uhd::direction_tx, D(direction_tx));
}
