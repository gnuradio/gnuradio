/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_MB_EXCEPTION_H
#define INCLUDED_MB_EXCEPTION_H

#include <stdexcept>

class mb_mblock;


class mbe_base : public std::logic_error
{
public:
  mbe_base(mb_mblock *mb, const std::string &msg);
};

class mbe_not_implemented : public mbe_base
{
public:
  mbe_not_implemented(mb_mblock *mb, const std::string &msg);
};


class mbe_no_such_component : public mbe_base
{
public:
  mbe_no_such_component(mb_mblock *, const std::string &component_name);
};

class mbe_duplicate_component : public mbe_base
{
public:
  mbe_duplicate_component(mb_mblock *, const std::string &component_name);
};

class mbe_no_such_port : public mbe_base
{
public:
  mbe_no_such_port(mb_mblock *, const std::string &port_name);
};

class mbe_duplicate_port : public mbe_base
{
public:
  mbe_duplicate_port(mb_mblock *, const std::string &port_name);
};

class mbe_already_connected : public mbe_base
{
public:
  mbe_already_connected(mb_mblock *, const std::string &comp_name,
			const std::string &port_name);
};

class mbe_incompatible_ports : public mbe_base
{
public:
  mbe_incompatible_ports(mb_mblock *,
			 const std::string &comp1_name,
			 const std::string &port1_name,
			 const std::string &comp2_name,
			 const std::string &port2_name);
};

class mbe_invalid_port_type : public mbe_base
{
public:
  mbe_invalid_port_type(mb_mblock *, const std::string &comp_name,
			const std::string &port_name);
};



#endif /* INCLUDED_MB_EXCEPTION_H */
