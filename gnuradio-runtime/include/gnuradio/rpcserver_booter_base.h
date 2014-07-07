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

#ifndef RPCSERVER_BOOTER_BASE
#define RPCSERVER_BOOTER_BASE

#include <string>
#include <vector>

class rpcserver_base;

class rpcserver_booter_base
{
public:
  rpcserver_booter_base() {;}
  virtual ~rpcserver_booter_base() {;}

  virtual rpcserver_base* i()=0;
  virtual const std::vector<std::string> endpoints()=0;
  virtual const std::string& type()=0;

private:
};

#endif /* RPCSERVER_BOOTER_BASE */
