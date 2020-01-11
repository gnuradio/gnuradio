/*
 * Copyright 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */
#include <gnuradio/log/stderr_backend.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>

namespace gr {
namespace log {
bool stderr_backend::log(const entry& what)
{
    auto time = std::chrono::system_clock::to_time_t(what.time);
    auto& localtime = *std::localtime(&time);
    std::cerr << "@" << std::put_time(&localtime, "%T") << " "
              << severity_to_string(what.level) << " [" << (what.source) << "]: ";
    if (what.purpose.size()) {
        std::cerr << "(" << (what.purpose) << ") ";
    }
    std::cerr << (what.message) << "\n";
    // std::cerr.flush();
    return std::cerr.good();
}
bool stderr_backend::flush()
{
    std::cerr.flush();
    return std::cerr.good();
}
const std::string stderr_backend::description()
{
    static const std::string desc("Logs to stderr");
    return desc;
}
stderr_backend::~stderr_backend()
{
    // NOP
}
} // namespace log
} // namespace gr
