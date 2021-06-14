/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <pybind11/complex.h>
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/uhd/uhd_types.h>

void bind_uhd_types(py::module& m)
{
    // Bindings copied from UHD

    // using str_map = std::map<std::string, std::string>;

    py::class_<::uhd::device_addr_t>(m, "device_addr_t")
        // Constructors
        .def(py::init<>())
        .def(py::init<std::string>())
        // Not defined for UHD < 3.11
        // .def(py::init<str_map>())

        // Methods
        .def("__str__", &uhd::device_addr_t::to_pp_string)
        .def("to_string", &uhd::device_addr_t::to_string)
        .def("to_pp_string", &uhd::device_addr_t::to_pp_string);

    // This will allow functions in Python that take a device_addr to also take
    // a string:
    py::implicitly_convertible<std::string, uhd::device_addr_t>();

    using stream_args_t = ::uhd::stream_args_t;
    py::class_<stream_args_t>(m, "stream_args_t")
        .def(py::init([](const std::string& cpu,
                         const std::string& otw,
                         const ::uhd::device_addr_t& args,
                         const std::vector<size_t>& channels) {
                 auto s = ::uhd::stream_args_t(cpu, otw);
                 s.args = args;
                 s.channels = channels;
                 return s;
             }),
             py::arg("cpu_format") = "",
             py::arg("otw_format") = "",
             py::arg("args") = ::uhd::device_addr_t(""),
             py::arg("channels") = std::vector<size_t>())
        // Properties
        .def_readwrite("cpu_format", &stream_args_t::cpu_format)
        .def_readwrite("otw_format", &stream_args_t::otw_format)
        .def_readwrite("args", &stream_args_t::args)
        .def_readwrite("channels", &stream_args_t::channels);


    // ::uhd::time_spec_t
    using time_spec_t = ::uhd::time_spec_t;

    py::class_<time_spec_t>(m, "time_spec_t")
        // Additional constructors
        .def(py::init<double>(), py::arg("secs") = 0)
        .def(py::init<int64_t, double>(), py::arg("full_secs"), py::arg("frac_secs") = 0)
        .def(py::init<int64_t, long, double>(),
             py::arg("full_secs"),
             py::arg("tick_count"),
             py::arg("tick_rate"))

        // Methods
        .def_static("from_ticks", &time_spec_t::from_ticks)

        .def("get_tick_count", &time_spec_t::get_tick_count)
        .def("to_ticks", &time_spec_t::to_ticks)
        .def("get_real_secs", &time_spec_t::get_real_secs)
        .def("get_full_secs", &time_spec_t::get_full_secs)
        .def("get_frac_secs", &time_spec_t::get_frac_secs)

        .def(py::self += time_spec_t())
        .def(py::self -= time_spec_t())
        .def(py::self + time_spec_t())
        .def(py::self - time_spec_t())
        .def(py::self += double())
        .def(py::self -= double())
        .def(py::self + double())
        .def(py::self - double());

    // ::uhd::stream_cmd_t
    using stream_cmd_t = ::uhd::stream_cmd_t;
    py::class_<stream_cmd_t>(m, "stream_cmd_t")
        .def(py::init<stream_cmd_t::stream_mode_t>())
        // Properties
        .def_readwrite("stream_mode", &stream_cmd_t::stream_mode)
        .def_readwrite("num_samps", &stream_cmd_t::num_samps)
        .def_readwrite("time_spec", &stream_cmd_t::time_spec)
        .def_readwrite("stream_now", &stream_cmd_t::stream_now);

    // ::uhd::stream_cmd_t::stream_mode_t
    py::enum_<stream_cmd_t::stream_mode_t>(m, "stream_mode_t")
        .value("STREAM_MODE_START_CONTINUOUS",
               stream_cmd_t::stream_mode_t::STREAM_MODE_START_CONTINUOUS)
        .value("STREAM_MODE_STOP_CONTINUOUS",
               stream_cmd_t::stream_mode_t::STREAM_MODE_STOP_CONTINUOUS)
        .value("STREAM_MODE_NUM_SAMPS_AND_DONE",
               stream_cmd_t::stream_mode_t::STREAM_MODE_NUM_SAMPS_AND_DONE)
        .value("STREAM_MODE_NUM_SAMPS_AND_MORE",
               stream_cmd_t::stream_mode_t::STREAM_MODE_NUM_SAMPS_AND_MORE)
        .export_values();

    // ::uhd::meta_range_t
    using meta_range_t = ::uhd::meta_range_t;

    py::class_<meta_range_t>(m, "meta_range_t")
        // Constructors
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, double>())

        // Methods
        .def("start", &meta_range_t::start)
        .def("stop", &meta_range_t::stop)
        .def("step", &meta_range_t::step)
        .def("clip", &meta_range_t::clip, py::arg("value"), py::arg("clip_step") = false)
        .def("__str__", &meta_range_t::to_pp_string);

    // ::uhd::tune_result_t
    using tune_result_t = ::uhd::tune_result_t;
    py::class_<tune_result_t>(m, "tune_result_t")
        .def(py::init<>())
        .def_readwrite("clipped_rf_freq", &tune_result_t::clipped_rf_freq)
        .def_readwrite("target_rf_freq", &tune_result_t::target_rf_freq)
        .def_readwrite("actual_rf_freq", &tune_result_t::actual_rf_freq)
        .def_readwrite("target_dsp_freq", &tune_result_t::target_dsp_freq)
        .def_readwrite("actual_dsp_freq", &tune_result_t::actual_dsp_freq)
        .def("__str__", &tune_result_t::to_pp_string);

    // ::uhd::tune_request_t
    using tune_request_t = ::uhd::tune_request_t;
    py::class_<tune_request_t> tune_request(m, "tune_request_t");
    tune_request.def(py::init<double, double>())
        .def(py::init<double>(), py::arg("target_freq") = 0.0)
        .def_readwrite("target_freq", &tune_request_t::target_freq)
        .def_readwrite("rf_freq_policy", &tune_request_t::rf_freq_policy)
        .def_readwrite("dsp_freq_policy", &tune_request_t::dsp_freq_policy)
        .def_readwrite("rf_freq", &tune_request_t::rf_freq)
        .def_readwrite("dsp_freq", &tune_request_t::dsp_freq)
        .def_readwrite("args", &tune_request_t::args);

    // ::uhd::tune_request_t::policy_t
    using policy_t = ::uhd::tune_request_t::policy_t;
    py::enum_<policy_t>(tune_request, "policy_t")
        .value("POLICY_NONE", policy_t::POLICY_NONE)
        .value("POLICY_AUTO", policy_t::POLICY_AUTO)
        .value("POLICY_MANUAL", policy_t::POLICY_MANUAL)
        .export_values();

    // ::uhd::sensor_value_t
    using sensor_value_t = uhd::sensor_value_t;

    py::class_<sensor_value_t>(m, "sensor_value_t")
        // Constructors
        .def(py::init<const std::string&, bool, const std::string&, const std::string&>())
        .def(py::init<const std::string&,
                      signed,
                      const std::string&,
                      const std::string&>())
        .def(py::init<const std::string&,
                      double,
                      const std::string&,
                      const std::string&>())
        .def(py::init<const std::string&, const std::string&, const std::string&>())

        // Methods
        .def("to_bool", &sensor_value_t::to_bool)
        .def("to_int", &sensor_value_t::to_int)
        .def("to_real", &sensor_value_t::to_real)
        .def("__str__", &sensor_value_t::to_pp_string);

    // NOTE: UHD 3.9 has these properties defined as const so they cannot be
    //  set in this manner.
    // // Properties
    // .def_readwrite("name", &sensor_value_t::name)
    // .def_readwrite("value", &sensor_value_t::value)
    // .def_readwrite("unit", &sensor_value_t::unit)
    // .def_readwrite("type", &sensor_value_t::type);
}
