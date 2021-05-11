/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "soapy_common.h"

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

py::object cast_string_to_arginfo_type(gr::soapy::argtype_t argtype,
                                       const std::string& str)
{
    py::object ret;
    switch (argtype) {
    case SoapySDR::ArgInfo::BOOL:
        ret = py::bool_(string_to_setting<bool>(str));
        break;

    case SoapySDR::ArgInfo::INT:
        ret = py::int_(string_to_setting<int>(str));
        break;

    case SoapySDR::ArgInfo::FLOAT:
        ret = py::float_(string_to_setting<double>(str));
        break;

    default:
        ret = py::str(str);
        break;
    }

    return ret;
}

setting_info cast_pyobject_to_arginfo_string(py::object obj)
{
    setting_info info;

    if (py::isinstance<py::bool_>(obj)) {
        info.value = setting_to_string(bool(py::cast<py::bool_>(obj)));
        info.type = SoapySDR::ArgInfo::BOOL;
    } else if (py::isinstance<py::int_>(obj)) {
        info.value = setting_to_string(int(py::cast<py::int_>(obj)));
        info.type = SoapySDR::ArgInfo::INT;
    } else if (py::isinstance<py::float_>(obj)) {
        info.value = setting_to_string(double(py::cast<py::float_>(obj)));
        info.type = SoapySDR::ArgInfo::FLOAT;
    } else {
        info.value = py::str(obj);
        info.type = SoapySDR::ArgInfo::STRING;
    }

    return info;
}
