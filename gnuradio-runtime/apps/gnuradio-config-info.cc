/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnuradio/constants.h>
#include <gnuradio/sys_paths.h>
#include <gnuradio/prefs.h>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
#include <iostream>

namespace po = boost::program_options;
using boost::format;

int
main(int argc, char **argv)
{
  po::options_description desc((format("Program options: %1% [options]") % argv[0]).str());
  po::variables_map vm;

  desc.add_options()
    ("help,h", "print help message")
    ("prefix", "print GNU Radio installation prefix")
    ("sysconfdir", "print GNU Radio system configuration directory")
    ("prefsdir", "print GNU Radio preferences directory")
    ("userprefsdir", "print GNU Radio user preferences directory")
    ("prefs", "print GNU Radio preferences")
    ("builddate", "print GNU Radio build date (RFC2822 format)")
    ("enabled-components", "print GNU Radio build time enabled components")
    ("cc", "print GNU Radio C compiler version")
    ("cxx", "print GNU Radio C++ compiler version")
    ("cflags", "print GNU Radio CFLAGS")
    ("version,v", "print GNU Radio version")
    ;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  }
  catch (po::error& error){
    std::cerr << "Error: " << error.what() << std::endl << std::endl;
    std::cerr << desc << std::endl;
    return 1;
  }

  if(vm.size() == 0 || vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }

  if(vm.count("prefix"))
    std::cout << gr::prefix() << std::endl;

  if(vm.count("sysconfdir"))
    std::cout << gr::sysconfdir() << std::endl;

  if(vm.count("prefsdir"))
    std::cout << gr::prefsdir() << std::endl;

  if(vm.count("userprefsdir"))
    std::cout << gr::userconf_path() << std::endl;

  if(vm.count("prefs"))
    std::cout << gr::prefs::singleton()->to_string() << std::endl;

  if(vm.count("builddate"))
    std::cout << gr::build_date() << std::endl;

  if(vm.count("enabled-components"))
    std::cout << gr::build_time_enabled_components() << std::endl;

  if(vm.count("version"))
    std::cout << gr::version() << std::endl;

  if(vm.count("cc"))
    std::cout << gr::c_compiler() << std::endl;

  if(vm.count("cxx"))
    std::cout << gr::cxx_compiler() << std::endl;

  if(vm.count("cflags"))
    std::cout << gr::compiler_flags() << std::endl;

  return 0;
}
