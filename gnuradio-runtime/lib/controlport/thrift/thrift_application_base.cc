/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#include <gnuradio/thrift_application_base.h>

int thrift_application_common::d_reacquire_attributes(0);
bool thrift_application_common::d_main_called(false);
bool thrift_application_common::d_have_thrift_config(false);
boost::shared_ptr<boost::thread> thrift_application_common::d_thread;
std::string thrift_application_common::d_endpointStr("");

boost::shared_ptr<thrift_application_common>
thrift_application_common::Instance()
{
  static boost::shared_ptr<thrift_application_common>
    instance(new thrift_application_common());
  return instance;
}

int
thrift_application_common::run(int, char**)
{
  std::cerr << "thrift_application_common: run" << std::endl;
  d_thriftserver->serve();
  return EXIT_SUCCESS;
}
