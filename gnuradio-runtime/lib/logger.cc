/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 * Copyright 2021 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include <spdlog/common.h>
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <memory>

namespace gr {
logging::logging()
    : _default_backend(std::make_shared<spdlog::sinks::dist_sink_mt>()),
      _debug_backend(std::make_shared<spdlog::sinks::dist_sink_mt>())
{
    _default_backend->set_level(spdlog::level::from_str(
        prefs::singleton()->get_string("LOG", "log_level", "info")));
    _debug_backend->set_level(spdlog::level::from_str(
        prefs::singleton()->get_string("LOG", "debug_level", "info")));


    auto debug_console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
    _debug_backend->add_sink(debug_console_sink);

    const auto pref = prefs::singleton();
    const auto default_file = pref->get_string("LOG", "log_file", "");

    if (default_file == "stderr") {
        auto console_sink = std::make_shared<spdlog::sinks::stderr_color_sink_st>();
        _default_backend->add_sink(console_sink);
    } else if ((!default_file.empty()) && default_file != "stdout") {
        auto file_sink =
            std::make_shared<spdlog::sinks::basic_file_sink_st>(default_file, true);
        _default_backend->add_sink(file_sink);
    }

    if (default_file == "stdout") {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_st>();
        _default_backend->add_sink(console_sink);
    }
}

void logging::set_default_level(log_level level) { _default_backend->set_level(level); }
void logging::set_debug_level(log_level level) { _debug_backend->set_level(level); }

logging& logging::singleton()
{
    static logging the_only_one;
    return the_only_one;
}

spdlog::sink_ptr logging::default_backend() const { return _default_backend; }
void logging::add_default_sink(const spdlog::sink_ptr& sink)
{
    _default_backend->add_sink(sink);
};
void logging::add_debug_sink(const spdlog::sink_ptr& sink)
{
    _debug_backend->add_sink(sink);
};

void logging::add_default_console_sink()
{
    add_default_sink(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
}

void logging::add_debug_console_sink()
{
    add_debug_sink(std::make_shared<spdlog::sinks::stdout_color_sink_st>());
}

logger::logger(const std::string& logger_name)
    : _name(logger_name),
      d_logger(
          std::make_shared<spdlog::logger>(_name, logging::singleton().default_backend()))
{
    d_logger->set_level(logging::singleton().default_level());
    d_logger->set_pattern(logging::default_pattern);
}

log_level logger::get_level() const { return d_logger->level(); }

const std::string logger::get_string_level() const
{
    const auto view = spdlog::level::to_string_view(d_logger->level());
    return std::string(view.begin(), view.end());
}

void logger::get_level(std::string& level) const { level = get_string_level(); }
void logger::set_level(const log_level level) { d_logger->set_level(level); }

void logger::set_level(const std::string& level)
{
    auto parsed_level = spdlog::level::from_str(level);
    if (parsed_level == spdlog::level::off) {
        if (level == "notice") {
            parsed_level = spdlog::level::info;
        } else if (level == "warn") {
            // older versions of spdlog don't convert this themselves
            parsed_level = spdlog::level::warn;
        } else if (level == "crit" || level == "alert" || level == "fatal" ||
                   level == "emerg") {
            parsed_level = spdlog::level::critical;
        }
    }
    d_logger->set_level(parsed_level);
}

const std::string& logger::name() const { return _name; }
void logger::set_name(const std::string& name)
{
    _name = name;
    d_logger->set_pattern(logging::default_pattern);
}

bool configure_default_loggers(gr::logger_ptr& l,
                               gr::logger_ptr& d,
                               const std::string& name)
{
    if (l == nullptr) {
        l = std::make_shared<gr::logger_ptr::element_type>(name);
    }
    if (d == nullptr) {
        d = std::make_shared<gr::logger_ptr::element_type>(name);
    }
    l->set_level(logging::singleton().default_level());
    d->set_level(logging::singleton().debug_level());
    l->set_name(name);
    d->set_name(name);
    return true;
}

} /* namespace gr */
