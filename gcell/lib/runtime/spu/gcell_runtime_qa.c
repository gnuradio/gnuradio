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

#include <gcell/gc_declare_proc.h>
#include <gcell/spu/gc_delay.h>
#include <string.h>


static void
qa_nop(const gc_job_direct_args_t *input _UNUSED,
       gc_job_direct_args_t *output _UNUSED,
       const gc_job_ea_args_t *eaa _UNUSED)
{
}

GC_DECLARE_PROC(qa_nop, "qa_nop");

static int
sum_shorts(short *p, int nshorts)
{
  int total = 0;
  for (int i = 0; i < nshorts; i++)
    total += p[i];

  return total;
}

static void
qa_sum_shorts(const gc_job_direct_args_t *input _UNUSED,
	      gc_job_direct_args_t *output,
	      const gc_job_ea_args_t *eaa)
{
  for (unsigned int i = 0; i < eaa->nargs; i++){
    short *p = eaa->arg[i].ls_addr;
    int n = eaa->arg[i].get_size / sizeof(short);
    output->arg[i].s32 = sum_shorts(p, n);
    //printf("qa_sum_shorts(%p, %d) = %d\n",  p, n, output->arg[i].s32);
  }
}

GC_DECLARE_PROC(qa_sum_shorts, "qa_sum_shorts");

static void
write_seq(unsigned char *p, int nbytes, int counter)
{
  for (int i = 0; i < nbytes; i++)
    p[i] = counter++;
}

static void
qa_put_seq(const gc_job_direct_args_t *input,
	   gc_job_direct_args_t *output _UNUSED,
	   const gc_job_ea_args_t *eaa)
{
  int counter = input->arg[0].s32;

  for (unsigned int i = 0; i < eaa->nargs; i++){
    unsigned char *p = eaa->arg[i].ls_addr;
    int n = eaa->arg[i].put_size;
    write_seq(p, n, counter);
    counter += n;
  }
}

GC_DECLARE_PROC(qa_put_seq, "qa_put_seq");

static void
qa_copy(const gc_job_direct_args_t *input _UNUSED,
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

GC_DECLARE_PROC(qa_copy, "qa_copy");
