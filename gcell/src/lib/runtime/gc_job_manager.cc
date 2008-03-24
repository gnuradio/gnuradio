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
#include "gc_job_manager.h"
#include "gc_job_manager_impl.h"

gc_job_manager *
gc_make_job_manager(const gc_jm_options *options)
{
  return new gc_job_manager_impl(options);
}

gc_job_manager::gc_job_manager(const gc_jm_options *options)
{
  // nop
}

gc_job_manager::~gc_job_manager()
{
  // nop
}

void
gc_job_manager::set_debug(int debug)
{
  // nop
}

int
gc_job_manager::debug()
{
  return 0;
}
