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
    py::class_<SoapySDR::Range>(m, "range")
        // Constructors
        .def(py::init<>())
        .def(py::init<double, double>())
        .def(py::init<double, double, double>())

        // Methods
        .def("minimum", &SoapySDR::Range::minimum)
        .def("maximum", &SoapySDR::Range::maximum)
        .def("step", &SoapySDR::Range::step)

        .def("__str__", [](const SoapySDR::Range& range) -> std::string {
            std::string ret = "(minimum: )";
            ret += std::to_string(range.minimum());
            ret += ", maximum: ";
            ret += std::to_string(range.maximum());
            ret += ", step: ";
            ret += std::to_string(range.step());
            ret += ")";

            return ret;
        });

    // decltype needed because the "type" field is an anonymous enum
    py::enum_<decltype(SoapySDR::ArgInfo::type)>(m, "argtype")
        .value("BOOL", SoapySDR::ArgInfo::BOOL)
        .value("INT", SoapySDR::ArgInfo::INT)
        .value("FLOAT", SoapySDR::ArgInfo::FLOAT)
        .value("STRING", SoapySDR::ArgInfo::STRING)
        .export_values();

    py::class_<SoapySDR::ArgInfo>(m, "arginfo")
        // Constructors
        .def(py::init<>())

        // Properties
        .def_readwrite("key", &SoapySDR::ArgInfo::key)
        .def_property(
            "value",
            [](const SoapySDR::ArgInfo& arginfo) -> py::object {
                py::object ret;
                switch (arginfo.type) {
                case SoapySDR::ArgInfo::BOOL:
                    ret = py::bool_(string_to_setting<bool>(arginfo.value));
                    break;

                case SoapySDR::ArgInfo::INT:
                    ret = py::int_(string_to_setting<int>(arginfo.value));
                    break;

                case SoapySDR::ArgInfo::FLOAT:
                    ret = py::float_(string_to_setting<double>(arginfo.value));
                    break;

                default:
                    ret = py::str(arginfo.value);
                    break;
                }

                return ret;
            },
            // So we can implicitly convert to Soapy's convention
            [](SoapySDR::ArgInfo& arginfo, py::object obj) -> void {
                if (py::isinstance<py::bool_>(obj)) {
                    arginfo.value = setting_to_string(bool(py::cast<py::bool_>(obj)));
                    arginfo.type = SoapySDR::ArgInfo::BOOL;
                } else if (py::isinstance<py::int_>(obj)) {
                    arginfo.value = setting_to_string(int(py::cast<py::int_>(obj)));
                    arginfo.type = SoapySDR::ArgInfo::INT;
                } else if (py::isinstance<py::float_>(obj)) {
                    arginfo.value = setting_to_string(double(py::cast<py::float_>(obj)));
                    arginfo.type = SoapySDR::ArgInfo::FLOAT;
                } else {
                    arginfo.value = py::str(obj);
                    arginfo.type = SoapySDR::ArgInfo::STRING;
                }
            })

        .def_readwrite("name", &SoapySDR::ArgInfo::name)
        .def_readwrite("description", &SoapySDR::ArgInfo::description)
        .def_readwrite("units", &SoapySDR::ArgInfo::units)
        .def_readwrite("type", &SoapySDR::ArgInfo::type)
        .def_readwrite("range", &SoapySDR::ArgInfo::range)
        .def_readwrite("options", &SoapySDR::ArgInfo::options)
        .def_readwrite("option_names", &SoapySDR::ArgInfo::optionNames)

        .def("__str__", [](const SoapySDR::ArgInfo& arginfo) -> std::string {
            return (arginfo.key + "=" + arginfo.value);
        });
}
