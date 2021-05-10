/*
 * Copyright 2021 Free Software Foundation, Inc.
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

#include <gnuradio/soapy/soapy_types.h>

#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.h>

// SoapySDR doesn't have an API define for SettingToString, so we need
// to check the version. 0.8 is the first tagged version to have this
// functionality.
#if SOAPY_SDR_API_VERSION >= 0x080000

template <typename T>
static inline T string_to_setting(const std::string& str)
{
    return SoapySDR::StringToSetting<T>(str);
}

template <typename T>
static inline std::string setting_to_string(const T& setting)
{
    return SoapySDR::SettingToString<T>(setting);
}

#else

// Copied from SoapySDR 0.8
#define SOAPY_SDR_TRUE "true"
#define SOAPY_SDR_FALSE "false"

#include <sstream>

template <typename T>
static inline T string_to_setting(const std::string& str)
{
    std::stringstream sstream(str);
    T setting;

    sstream >> setting;

    return setting;
}

// Copied from SoapySDR 0.8
template <>
inline bool string_to_setting<bool>(const std::string& str)
{
    if (str == SOAPY_SDR_TRUE)
        return true;
    if (str == SOAPY_SDR_FALSE)
        return false;

    // zeros and empty strings are false
    if (str == "0")
        return false;
    if (str == "0.0")
        return false;
    if (str == "")
        return false;

    // other values are true
    return true;
}

template <typename T>
static inline std::string setting_to_string(const T& setting)
{
    return std::to_string(setting);
}

template <>
inline std::string setting_to_string<bool>(const bool& setting)
{
    return setting ? SOAPY_SDR_TRUE : SOAPY_SDR_FALSE;
}

#endif

void bind_soapy_types(py::module& m)
{
    py::class_<gr::soapy::range_t>(m, "range_t")
        // Constructors
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, double>())

        // Methods
        .def("minimum", &gr::soapy::range_t::minimum)
        .def("maximum", &gr::soapy::range_t::maximum)
        .def("step", &gr::soapy::range_t::step)

        .def("__str__", [](const gr::soapy::range_t& range) -> std::string {
            std::string ret = "(minimum: ";
            ret += std::to_string(range.minimum());
            ret += ", maximum: ";
            ret += std::to_string(range.maximum());
            ret += ", step: ";
            ret += std::to_string(range.step());
            ret += ")";

            return ret;
        });

    py::enum_<gr::soapy::argtype_t>(m, "argtype_t")
        .value("BOOL", gr::soapy::arginfo_t::BOOL)
        .value("INT", gr::soapy::arginfo_t::INT)
        .value("FLOAT", gr::soapy::arginfo_t::FLOAT)
        .value("STRING", gr::soapy::arginfo_t::STRING)
        .export_values();

    py::class_<gr::soapy::arginfo_t>(m, "arginfo_t")
        // Constructors
        .def(py::init<>())

        // Properties
        .def_readwrite("key", &gr::soapy::arginfo_t::key)
        .def_property(
            "value",
            [](const gr::soapy::arginfo_t& arginfo) -> py::object {
                py::object ret;
                switch (arginfo.type) {
                case gr::soapy::arginfo_t::BOOL:
                    ret = py::bool_(string_to_setting<bool>(arginfo.value));
                    break;

                case gr::soapy::arginfo_t::INT:
                    ret = py::int_(string_to_setting<int>(arginfo.value));
                    break;

                case gr::soapy::arginfo_t::FLOAT:
                    ret = py::float_(string_to_setting<double>(arginfo.value));
                    break;

                default:
                    ret = py::str(arginfo.value);
                    break;
                }

                return ret;
            },
            // So we can implicitly convert to Soapy's convention
            [](gr::soapy::arginfo_t& arginfo, py::object obj) -> void {
                if (py::isinstance<py::bool_>(obj)) {
                    arginfo.value = setting_to_string(bool(py::cast<py::bool_>(obj)));
                    arginfo.type = gr::soapy::arginfo_t::BOOL;
                } else if (py::isinstance<py::int_>(obj)) {
                    arginfo.value = setting_to_string(int(py::cast<py::int_>(obj)));
                    arginfo.type = gr::soapy::arginfo_t::INT;
                } else if (py::isinstance<py::float_>(obj)) {
                    arginfo.value = setting_to_string(double(py::cast<py::float_>(obj)));
                    arginfo.type = gr::soapy::arginfo_t::FLOAT;
                } else {
                    arginfo.value = py::str(obj);
                    arginfo.type = gr::soapy::arginfo_t::STRING;
                }
            })

        .def_readwrite("name", &gr::soapy::arginfo_t::name)
        .def_readwrite("description", &gr::soapy::arginfo_t::description)
        .def_readwrite("units", &gr::soapy::arginfo_t::units)
        .def_readwrite("type", &gr::soapy::arginfo_t::type)
        .def_readwrite("range", &gr::soapy::arginfo_t::range)
        .def_readwrite("options", &gr::soapy::arginfo_t::options)
        .def_readwrite("option_names", &gr::soapy::arginfo_t::optionNames)

        .def("__str__", [](const gr::soapy::arginfo_t& arginfo) -> std::string {
            return (arginfo.key + "=" + arginfo.value);
        });
}
