/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_runtime_placeholder.h>
#include <mb_mblock.h>
#include <mb_exception.h>


static mb_runtime *s_singleton = 0;


mb_runtime_placeholder::mb_runtime_placeholder()
{
  // nop
}

mb_runtime_placeholder::~mb_runtime_placeholder()
{
  // nop
}

bool
mb_runtime_placeholder::run(mb_mblock_sptr top)
{
  throw mbe_not_implemented(top.get(), "mb_runtime_placeholder::run");
}

mb_runtime *
mb_runtime_placeholder::singleton()
{
  if (s_singleton)
    return s_singleton;

  s_singleton = new mb_runtime_placeholder();
  return s_singleton;
}
