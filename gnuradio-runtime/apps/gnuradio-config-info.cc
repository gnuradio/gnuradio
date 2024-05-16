/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
 * Copyright 2024 Marcus Müller
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
#include <gnuradio/prefs.h>
#include <gnuradio/sys_paths.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int main(int argc, char** argv)
{
    po::options_description desc("Program options: " + std::string(argv[0]) +
                                 " [options]");
    po::variables_map vm;

    // clang-format off
    desc.add_options()("help,h", "print help message")(
        "print-all", "print all information")(
        "prefix", "print GNU Radio installation prefix")(
        "sysconfdir", "print GNU Radio system configuration directory")(
        "prefsdir", "print GNU Radio preferences directory")(
        "userprefsdir", "print GNU Radio user preferences directory")(
        "persistentdir", "print GNU Radio persistent state directory")(
        "prefs", "print GNU Radio preferences")(
        "builddate", "print GNU Radio build date (RFC2822 format)")(
        "enabled-components", "print GNU Radio build time enabled components")(
        "cc", "print GNU Radio C compiler version")(
        "cxx", "print GNU Radio C++ compiler version")(
        "cflags", "print GNU Radio CFLAGS")("version,v", "print GNU Radio version")(
        "pybind", "print pybind11 version used in this build");
    // clang-format on

    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    } catch (po::error& error) {
        std::cerr << "Error: " << error.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    if (vm.empty() || vm.count("help")) {
        std::cout << desc << std::endl;
        return 1;
    }

    bool print_all = vm.count("print-all");

    if (vm.count("prefix") || print_all)
        std::cout << gr::prefix() << std::endl;

    if (vm.count("sysconfdir") || print_all)
        std::cout << gr::sysconfdir() << std::endl;

    if (vm.count("prefsdir") || print_all)
        std::cout << gr::prefsdir() << std::endl;

    if (vm.count("userprefsdir") || print_all)
        std::cout << gr::paths::userconf().string() << std::endl;

    if (vm.count("persistentdir") || print_all)
        std::cout << gr::paths::persistent().string() << std::endl;

    // Not included in print all due to verbosity
    if (vm.count("prefs"))
        std::cout << gr::prefs::singleton()->to_string() << std::endl;

    if (vm.count("builddate") || print_all)
        std::cout << gr::build_date() << std::endl;

    if (vm.count("enabled-components") || print_all)
        std::cout << gr::build_time_enabled_components() << std::endl;

    if (vm.count("version") || print_all)
        std::cout << gr::version() << std::endl;

    if (vm.count("cc") || print_all)
        std::cout << gr::c_compiler() << std::endl;

    if (vm.count("cxx") || print_all)
        std::cout << gr::cxx_compiler() << std::endl;

    if (vm.count("cflags") || print_all)
        std::cout << gr::compiler_flags() << std::endl;

    if (vm.count("pybind") || print_all)
        std::cout << gr::pybind_version() << std::endl;

    return 0;
}
