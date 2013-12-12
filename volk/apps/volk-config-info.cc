/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#include <volk/constants.h>
#include "volk/volk.h"
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int
main(int argc, char **argv)
{
  po::options_description desc("Program options: volk-config-info [options]");
  po::variables_map vm;

  desc.add_options()
    ("help,h", "print help message")
    ("prefix", "print VOLK installation prefix")
    ("builddate", "print VOLK build date (RFC2822 format)")
    ("cc", "print VOLK C compiler version")
    ("cflags", "print VOLK CFLAGS")
    ("all-machines", "print VOLK machines built into library")
    ("avail-machines", "print VOLK machines the current platform can use")
    ("machine", "print the VOLK machine that will be used")
    ("version,v", "print VOLK version")
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
    std::cout << volk_prefix() << std::endl;

  if(vm.count("builddate"))
    std::cout << volk_build_date() << std::endl;

  if(vm.count("version"))
    std::cout << volk_version() << std::endl;

  if(vm.count("cc"))
    std::cout << volk_c_compiler() << std::endl;

  if(vm.count("cflags"))
    std::cout << volk_compiler_flags() << std::endl;

  // stick an extra ';' to make output of this and avail-machines the
  // same structure for easier parsing
  if(vm.count("all-machines"))
    std::cout << volk_available_machines() << ";" << std::endl;

  if(vm.count("avail-machines")) {
    volk_list_machines();
  }

  if(vm.count("machine")) {
    std::cout << volk_get_machine() << std::endl;
  }

  return 0;
}
