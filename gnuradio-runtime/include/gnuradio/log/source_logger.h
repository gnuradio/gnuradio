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

#ifndef INCLUDED_GR_RUNTIME_RUNTIME_SOURCE_LOGGER_H
#define INCLUDED_GR_RUNTIME_RUNTIME_SOURCE_LOGGER_H

#include <gnuradio/api.h>
#include <gnuradio/log/entry.h>
#include <gnuradio/log/log.h>
#include <sstream>
#include <string>
namespace gr {
namespace log {
class eom_t
{
};
class purpose_t
{
};
static eom_t eom;
static purpose_t msg;
template <severity level>
class GR_RUNTIME_API source_logger
{
public:
    source_logger(const std::string& src) : src(src), sent(false) {}
    template <typename T>
    source_logger& operator<<(T& what)
    {
        sent = false;
        string_builder << what;
        return *this;
    }
    source_logger& operator<<(eom_t&)
    {
        if (!sent) {
            instance()(entry(level, src, purpose, string_builder.str()));
            sent = true;
        }
        return *this;
    }
    source_logger& operator<<(purpose_t&)
    {
        purpose = string_builder.str();
        string_builder.str(std::string());
        string_builder.clear();
        sent = true;
        return *this;
    }
    ~source_logger()
    {
        if (!sent) {
            instance()(entry(level, src, purpose, string_builder.str()));
        }
    }

private:
    const std::string src;
    const std::string purpose;
    bool sent;
    std::stringstream string_builder;
};
template <>
class GR_RUNTIME_API source_logger<info>
{
public:
    source_logger(const std::string&)
    { /*NOP*/
    }
};
} // namespace log
} // namespace gr
#endif /* INCLUDED_GR_RUNTIME_RUNTIME_LOG_SOURCE_LOGGER_H */
