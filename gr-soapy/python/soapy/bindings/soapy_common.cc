/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "setting_string_conversion.h"
#include "soapy_common.h"

#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.h>
#include <string>

py::object cast_string_to_arginfo_type(gr::soapy::argtype_t argtype,
                                       const std::string& str)
{
    py::object ret;
    switch (argtype) {
    case SoapySDR::ArgInfo::BOOL:
        ret = py::bool_(gr::soapy::string_to_setting<bool>(str));
        break;

    case SoapySDR::ArgInfo::INT:
        ret = py::int_(gr::soapy::string_to_setting<int>(str));
        break;

    case SoapySDR::ArgInfo::FLOAT:
        ret = py::float_(gr::soapy::string_to_setting<double>(str));
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
        info.value = gr::soapy::setting_to_string(bool(py::cast<py::bool_>(obj)));
        info.type = SoapySDR::ArgInfo::BOOL;
    } else if (py::isinstance<py::int_>(obj)) {
        info.value = gr::soapy::setting_to_string(int(py::cast<py::int_>(obj)));
        info.type = SoapySDR::ArgInfo::INT;
    } else if (py::isinstance<py::float_>(obj)) {
        info.value = gr::soapy::setting_to_string(double(py::cast<py::float_>(obj)));
        info.type = SoapySDR::ArgInfo::FLOAT;
    } else {
        info.value = py::str(obj);
        info.type = SoapySDR::ArgInfo::STRING;
    }

    return info;
}
