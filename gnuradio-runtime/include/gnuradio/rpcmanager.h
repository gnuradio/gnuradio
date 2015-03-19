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

#ifndef RPCMANAGER_H
#define RPCMANAGER_H

#include <gnuradio/api.h>
#include <gnuradio/rpcmanager_base.h>
#include <gnuradio/rpcserver_booter_aggregator.h>
#include <memory>
#include <iostream>

class GR_RUNTIME_API rpcmanager : public virtual rpcmanager_base
{
 public:
  rpcmanager();
  ~rpcmanager();

  static rpcserver_booter_base* get();

  static void register_booter(rpcserver_booter_base* booter);

  template<typename T> class rpcserver_booter_register_helper
  {
  public:
    rpcserver_booter_register_helper() {
      rpcmanager::register_booter(new T());
    }

    //TODO: unregister
  };

 private:
  static bool make_aggregator;
  static bool booter_registered;
  static bool aggregator_registered;
  static void rpcserver_booter_base_sptr_dest(rpcserver_booter_base* b) {;}
  static std::auto_ptr<rpcserver_booter_base> boot;
  static std::auto_ptr<rpcserver_booter_aggregator> aggregator;
};

#endif /* RPCMANAGER_H */
