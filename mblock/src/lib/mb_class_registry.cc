/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_class_registry.h>
#include <map>

static std::map<std::string, mb_mblock_maker_t>	s_registry;

bool
mb_class_registry::register_maker(const std::string &name, mb_mblock_maker_t maker)
{
  s_registry[name] = maker;
  return true;
}

bool
mb_class_registry::lookup_maker(const std::string &name, mb_mblock_maker_t *maker)
{
  if (s_registry.count(name) == 0){  // not registered
    *maker = (mb_mblock_maker_t) 0;
    return false;
  }

  *maker = s_registry[name];
  return true;
}
