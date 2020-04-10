/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/constants.h>
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;
using boost::format;

int main(int argc, char** argv)
{
    po::options_description desc(
        (format("Program options: %1% [options]") % argv[0]).str());
    po::variables_map vm;

    desc.add_options()("help,h", "print help message")(
        "prefix", "print GNU Radio installation prefix")(
        "sysconfdir", "print GNU Radio system configuration directory")(
        "prefsdir", "print GNU Radio preferences directory")(
        "userprefsdir", "print GNU Radio user preferences directory")(
        "prefs", "print GNU Radio preferences")(
        "builddate", "print GNU Radio build date (RFC2822 format)")(
        "enabled-components", "print GNU Radio build time enabled components")(
        "cc", "print GNU Radio C compiler version")(
        "cxx", "print GNU Radio C++ compiler version")(
        "cflags", "print GNU Radio CFLAGS")("version,v", "print GNU Radio version");

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error& error) {
        gr::logger_ptr logger, debug_logger;
        gr::configure_default_loggers(logger, debug_logger, "gnuradio-config-info.cc");
        GR_LOG_ERROR(logger, boost::format("ERROR %s %s") % error.what() % desc);
        return 1;
    }

    if (vm.empty() || vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    if (vm.count("prefix"))
        std::cout << gr::prefix() << std::endl;

    if (vm.count("sysconfdir"))
        std::cout << gr::sysconfdir() << std::endl;

    if (vm.count("prefsdir"))
        std::cout << gr::prefsdir() << std::endl;

    if (vm.count("userprefsdir"))
        std::cout << gr::userconf_path() << std::endl;

    if (vm.count("prefs"))
        std::cout << gr::prefs::singleton()->to_string() << std::endl;

    if (vm.count("builddate"))
        std::cout << gr::build_date() << std::endl;

    if (vm.count("enabled-components"))
        std::cout << gr::build_time_enabled_components() << std::endl;

    if (vm.count("version"))
        std::cout << gr::version() << std::endl;

    if (vm.count("cc"))
        std::cout << gr::c_compiler() << std::endl;

    if (vm.count("cxx"))
        std::cout << gr::cxx_compiler() << std::endl;

    if (vm.count("cflags"))
        std::cout << gr::compiler_flags() << std::endl;

    return 0;
}
