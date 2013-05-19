/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <gnuradio/ice_application_base.h>

int ice_application_common::d_reacquire_attributes(0);
bool ice_application_common::d_main_called(false);
bool ice_application_common::d_have_ice_config(false);
boost::shared_ptr<boost::thread> ice_application_common::d_thread;
std::string ice_application_common::d_endpointStr("");

boost::shared_ptr<ice_application_common>
ice_application_common::Instance()
{
  static boost::shared_ptr<ice_application_common>
    instance(new ice_application_common());
  return instance;
}

int ice_application_common::run(int, char**)
{
  communicator()->waitForShutdown();
  return EXIT_SUCCESS;
}
