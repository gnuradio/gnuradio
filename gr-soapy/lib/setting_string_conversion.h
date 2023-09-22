/*
 * Copyright 2021 Nicholas Corgan
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_GR_SOAPY_SETTING_STRING_CONVERSION_H
#define INCLUDED_GR_SOAPY_SETTING_STRING_CONVERSION_H

#include <SoapySDR/Types.hpp>
#include <SoapySDR/Version.h>

#include <sstream>

namespace gr {
namespace soapy {

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

template <typename T>
static inline T string_to_setting(const std::string& str)
{
    std::stringstream sstream(str);
    T setting;

    sstream >> setting;

    return setting;
}

// Copied from SoapySDR 0.8
//! convert empty and "false" strings to false, integers to their truthness
template <>
inline bool string_to_setting<bool>(const std::string& str)
{
    if (str.empty() || str == SOAPY_SDR_FALSE) {
        return false;
    }
    if (str == SOAPY_SDR_TRUE) {
        return true;
    }
    try {
        return static_cast<bool>(std::stod(str));
    } catch (std::invalid_argument& e) {
    }
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

} // namespace soapy
} // namespace gr

#endif /* INCLUDED_GR_SOAPY_SETTING_STRING_CONVERSION_H */
