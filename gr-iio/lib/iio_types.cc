/* -*- c++ -*- */
/*
 * Copyright 2021 Josh Morman
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <gnuradio/iio/iio_types.h>
#include <sstream>
#include <string>

namespace gr {
namespace iio {


iio_param_t::iio_param_t(const std::string& key, iio_param_value_t value)
{
    this->first = key;
    this->second = to_string(value);
}
iio_param_t::iio_param_t(const std::string& kvpair)
{
    std::istringstream ss(kvpair);
    std::string tok;
    std::vector<std::string> toks;
    while (getline(ss, tok, '=')) {
        toks.push_back(tok);
    }
    if (toks.size() == 2 && !toks[0].empty()) { // only valid combination
        this->first = toks[0];
        this->second = toks[1];
    } else
        throw std::invalid_argument("invalid parameter string: " +
                                    kvpair); // otherwise error
}

std::string iio_param_t::to_string(iio_param_value_t value)
{
    // long long unsigned int, long unsigned int, int, double, std::string
    if (std::holds_alternative<long long unsigned int>(value)) {
        return std::to_string(std::get<long long unsigned int>(value));
    } else if (std::holds_alternative<long unsigned int>(value)) {
        return std::to_string(std::get<long unsigned int>(value));
    } else if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<double>(value)) {
        std::string value_string = std::to_string(std::get<double>(value));
        std::string::size_type idx = value_string.find(',');
        if (idx != std::string::npos) // found , as decimal separator, so change to .
            value_string.replace(idx, 1, ".");
        return value_string;
    } else {
        return std::get<std::string>(value);
    }
}


} // namespace iio
} // namespace gr
