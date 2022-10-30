/*
 * Copyright 2022 Marcus Müller
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */
#include <gnuradio/dictionary_logger_backend.h>

#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>

#include <spdlog/sinks/base_sink.h>
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <memory>
#include <stdexcept>

BOOST_AUTO_TEST_CASE(t1)
{
    gr::prefs::singleton()->set_string("LOG", "log_level", "info");
    auto& logging_system = gr::logging::singleton();

    auto backend = std::make_shared<gr::dictionary_logger_backend>(); // unfiltered

    BOOST_CHECK(backend);
    logging_system.add_default_sink(backend);

    constexpr auto logger_name = "foobar!";
    constexpr auto ignored_logger_name = "ignoreme";
    constexpr auto logger_template = "Number {:06d}";
    size_t n_messages = 1000;

    auto logger = gr::logger(logger_name);
    logger.set_level(gr::log_level::info);
    auto ignore_logger = gr::logger(ignored_logger_name);
    ignore_logger.set_level(gr::log_level::info);
    ignore_logger.info("we don't care about this.");

    for (unsigned int i = 0; i < n_messages; ++i) {
        logger.info(logger_template, i);
    }

    auto map = backend->get_map();
    BOOST_CHECK(map.size() >= 2);
    try {
        const auto& logs = map.at(logger_name);

        size_t counter = 0;
        for (const auto& msg : logs) {
            BOOST_CHECK(fmt::format(logger_template, counter) == msg.second);
            ++counter;
        }
    } catch (const std::out_of_range& ex) {
        // something unexpected happened. Let's still dump all loggers, in hopes it helps
        // in any debug efforts
        for (const auto& logger : map) {
            fmt::print(stderr, "[{}]\n", logger.first);
        }
        BOOST_CHECK(false);
    }
}

BOOST_AUTO_TEST_CASE(t2)
{
    gr::prefs::singleton()->set_string("LOG", "log_level", "info");
    auto& logging_system = gr::logging::singleton();

    // Filter: ignore anything starting with "ignore"
    auto backend = std::make_shared<gr::dictionary_logger_backend>(
        std::regex("^Privilege.*", std::regex::basic));

    BOOST_CHECK(backend);
    logging_system.add_default_sink(backend);

    constexpr auto logger_name = "Privileged Logger";
    constexpr auto ignored_logger_name = "Unprivileged Logger";
    constexpr auto logger_template = "Number {:06d}";
    size_t n_messages = 1000;

    auto logger = gr::logger(logger_name);
    logger.set_level(gr::log_level::info);
    auto ignore_logger = gr::logger(ignored_logger_name);
    ignore_logger.set_level(gr::log_level::info);
    ignore_logger.info("we don't care about this.");

    for (unsigned int i = 0; i < n_messages; ++i) {
        logger.info(logger_template, i);
    }

    auto map = backend->get_map();
    // rejection must work
    BOOST_CHECK_THROW(map.at(ignored_logger_name), std::out_of_range);

    try {
        size_t counter = 0;
        const auto& logs = map.at(logger_name);
        for (const auto& msg : logs) {
            BOOST_CHECK(fmt::format(logger_template, counter) == msg.second);
            ++counter;
        }
    } catch (const std::out_of_range& ex) {
        // something unexpected happened. Let's still dump all loggers, in hopes it helps
        // in any debug efforts
        for (const auto& logger : map) {
            fmt::print(stderr, "[{}]\n", logger.first);
        }
        BOOST_CHECK(false);
    }
}
