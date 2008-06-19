/* -*- c++ -*- */
/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#if defined(HAVE_CONFIG_H)
#include <config.h>
#endif
#include "gc_job_manager.h"
#include <omni_time.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

// handle to embedded SPU executable that contains benchmark routines
// (The name of the variable (benchmark_procs) is the name of the spu executable.)
extern spe_program_handle_t benchmark_procs;

static gc_proc_id_t gcp_benchmark_udelay = GCP_UNKNOWN_PROC;

static void
init_jd(gc_job_desc *jd, unsigned int usecs)
{
  jd->proc_id = gcp_benchmark_udelay;
  jd->input.nargs = 1;
  jd->input.arg[0].u32 = usecs;
  jd->output.nargs = 0;
  jd->eaa.nargs = 0;
}

static void
run_test(unsigned int nspes, unsigned int usecs, int njobs)
{
  static const int NJDS = 64;
  int nsubmitted = 0;
  int ncompleted = 0;
  gc_job_desc *all_jds[NJDS];
  gc_job_desc *jds[2][NJDS];
  unsigned int njds[2];
  unsigned int ci;		// current index
  bool done[NJDS];
  
  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&benchmark_procs);
  opts.nspes = nspes;
  opts.gang_schedule = true;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

  if ((gcp_benchmark_udelay = mgr->lookup_proc("benchmark_udelay")) == GCP_UNKNOWN_PROC){
    fprintf(stderr, "lookup_proc: failed to find \"benchmark_udelay\"\n");
    return;
  }

  // allocate and init all job descriptors
  for (int i = 0; i < NJDS; i++){
    all_jds[i] = mgr->alloc_job_desc();
    init_jd(all_jds[i], usecs);
  }

  omni_time t_start = omni_time::time();

  ci = 0;
  njds[0] = 0;
  njds[1] = 0;
  
  // submit the first batch
  for (int i = 0; i < NJDS; i++){
    if (mgr->submit_job(all_jds[i])){
      jds[ci][njds[ci]++] = all_jds[i];
      nsubmitted++;
    }
    else {
      printf("submit_job(jds[%d]) failed, status = %d\n",
	     i, all_jds[i]->status);
    }
  }
  
  while (ncompleted < njobs){
    njds[ci^1] = 0;
    int n = mgr->wait_jobs(njds[ci], jds[ci], done, GC_WAIT_ANY);
    // printf("%2d\n", n);
    if (n < 0){
      fprintf(stderr, "mgr->wait_jobs failed\n");
      break;
    }
    for (unsigned int i = 0; i < njds[ci]; i++){
      if (!done[i]){	// remember for next iteration
	jds[ci^1][njds[ci^1]++] = jds[ci][i];
      }
      else {
	ncompleted++;
	// printf("ncompleted = %7d\n", ncompleted);
	if (nsubmitted < njobs){		    // submit another one
	  if (mgr->submit_job(jds[ci][i])){
	    jds[ci^1][njds[ci^1]++] = jds[ci][i];  // remember for next iter
	    nsubmitted++;
	  }
	  else {
	    printf("submit_job(jds[%d]) failed, status = %d\n",
		   i, jds[ci][i]->status);
	  }
	}
      }
    }
    ci ^= 1;	// toggle current
  }

  // stop timing
  omni_time t_stop = omni_time::time();
  double delta = (t_stop - t_start).double_time();
  printf("nspes: %2d  udelay: %4d  elapsed_time: %7.3f  njobs: %g  speedup: %6.3f\n",
	 mgr->nspes(), usecs, delta, (double) njobs,
	 njobs * usecs * 1e-6 / delta);
}

int
main(int argc, char **argv)
{
  unsigned int nspes = 0;
  unsigned int usecs = 0;
  int njobs = 500000;
  int ch;

  while ((ch = getopt(argc, argv, "n:u:N:")) != EOF){
    switch(ch){
    case 'n':
      nspes = strtol(optarg, 0, 0);
      break;

    case 'u':
      usecs = strtol(optarg, 0, 0);
      break;

    case 'N':
      njobs = strtol(optarg, 0, 0);
      break;

    case '?':
    default:
      fprintf(stderr, "usage: benchmark_nop [-n <nspes>] [-u <udelay>] [-N <njobs>]\n");
      return 1;
    }
  }

  run_test(nspes, usecs, njobs);
  return 0;
}
