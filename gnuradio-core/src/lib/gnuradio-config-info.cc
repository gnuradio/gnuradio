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

#include <gr_constants.h>
#include <boost/program_options.hpp>
#include <iostream>

namespace po = boost::program_options;

int
main(int argc, char **argv)
{
  po::options_description desc("Program options: gnuradio [options]");
  po::variables_map vm;

  desc.add_options()
    ("help,h", "print help message")
    ("prefix", "print gnuradio installation prefix")
    ("sysconfdir", "print gnuradio system configuration directory")
    ("prefsdir", "print gnuradio preferences directory")
    ("builddate", "print gnuradio build date (RFC2822 format)")
    ("version,v", "print gnuradio version")
    ;

  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.size() == 0 || vm.count("help")) {
    std::cout << desc << std::endl;
    return 1;
  }
      
  if (vm.count("prefix"))
    std::cout << gr_prefix() << std::endl;

  if (vm.count("sysconfdir"))
    std::cout << gr_sysconfdir() << std::endl;

  if (vm.count("prefsdir"))
    std::cout << gr_prefsdir() << std::endl;

  if (vm.count("builddate"))
    std::cout << gr_build_date() << std::endl;

  if (vm.count("version"))
    std::cout << gr_version() << std::endl;

  return 0;
}
