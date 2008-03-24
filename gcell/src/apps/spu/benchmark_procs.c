/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <gc_delay.h>
#include <gc_declare_proc.h>
#include <string.h>

#define _UNUSED __attribute__((unused))


static void
benchmark_udelay(const gc_job_direct_args_t *input,
		 gc_job_direct_args_t *output _UNUSED,
		 const gc_job_ea_args_t *eaa _UNUSED)
{
  gc_udelay(input->arg[0].u32);
}

GC_DECLARE_PROC(benchmark_udelay, "benchmark_udelay");



static void
benchmark_put_zeros(const gc_job_direct_args_t *input _UNUSED,
		    gc_job_direct_args_t *output _UNUSED,
		    const gc_job_ea_args_t *eaa)
{
  for (unsigned int i = 0; i < eaa->nargs; i++){
    if (eaa->arg[i].direction == GCJD_DMA_PUT)
      memset(eaa->arg[i].ls_addr, 0, eaa->arg[i].put_size);
  }
}

GC_DECLARE_PROC(benchmark_put_zeros, "benchmark_put_zeros");


static void
benchmark_copy(const gc_job_direct_args_t *input _UNUSED,
	       gc_job_direct_args_t *output,
	       const gc_job_ea_args_t *eaa)
{
  if (eaa->nargs != 2
      || eaa->arg[0].direction != GCJD_DMA_PUT
      || eaa->arg[1].direction != GCJD_DMA_GET){
    output->arg[0].s32 = -1;
    return;
  }

  output->arg[0].s32 = 0;
  unsigned n = eaa->arg[0].put_size;
  if (eaa->arg[1].get_size < n)
    n = eaa->arg[1].get_size;
  
  memcpy(eaa->arg[0].ls_addr, eaa->arg[1].ls_addr, n);
}

GC_DECLARE_PROC(benchmark_copy, "benchmark_copy");
