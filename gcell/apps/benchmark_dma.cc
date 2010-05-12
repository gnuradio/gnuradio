/* -*- c++ -*- */
/*
 * Copyright 2007,2008,2010 Free Software Foundation, Inc.
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
#include <gcell/gc_job_manager.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <boost/scoped_array.hpp>
#include <assert.h>

// handle to embedded SPU executable that contains benchmark routines
// (The name of the variable (benchmark_procs) is the name of the spu executable.)
extern spe_program_handle_t benchmark_procs;

static gc_proc_id_t gcp_benchmark_udelay = GCP_UNKNOWN_PROC;

#define	BENCHMARK_PUT		0x1
#define	BENCHMARK_GET		0x2
#define	BENCHMARK_GET_PUT	(BENCHMARK_PUT|BENCHMARK_GET)


#if 0
static bool
power_of_2_p(unsigned long x)
{
  int nbits = sizeof(x) * 8;
  for (int i = 0; i < nbits; i++)
    if (x == (1UL << i))
      return true;

  return false;
}
#endif

static void
init_jd(gc_job_desc *jd, unsigned int usecs,
	unsigned char *getbuf, unsigned char *putbuf, size_t buflen,
	int getput_mask)
{
  jd->proc_id = gcp_benchmark_udelay;
  jd->input.nargs = 1;
  jd->input.arg[0].u32 = usecs;
  jd->output.nargs = 0;

  switch(getput_mask & BENCHMARK_GET_PUT){

  case BENCHMARK_GET:
    jd->eaa.nargs = 1;
    jd->eaa.arg[0].direction = GCJD_DMA_GET;
    jd->eaa.arg[0].ea_addr = ptr_to_ea(getbuf);
    jd->eaa.arg[0].get_size = buflen;
    break;

  case BENCHMARK_PUT:
    jd->eaa.nargs = 1;
    jd->eaa.arg[0].direction = GCJD_DMA_PUT;
    jd->eaa.arg[0].ea_addr = ptr_to_ea(putbuf);
    jd->eaa.arg[0].put_size = buflen;
    break;
    
  case BENCHMARK_GET_PUT:
    jd->eaa.nargs = 2;
    jd->eaa.arg[0].direction = GCJD_DMA_GET;
    jd->eaa.arg[0].ea_addr = ptr_to_ea(getbuf);
    jd->eaa.arg[0].get_size = buflen;
    jd->eaa.arg[1].direction = GCJD_DMA_PUT;
    jd->eaa.arg[1].ea_addr = ptr_to_ea(putbuf);
    jd->eaa.arg[1].put_size = buflen;
    break;
  }
}

static void
run_test(unsigned int nspes, unsigned int usecs, unsigned int dma_size, int getput_mask)
{
  using namespace boost::posix_time;

  static const int64_t TOTAL_SIZE_DMA = 5LL << 30;
  static const int NJDS = 64;
  unsigned int njobs = (unsigned int)(TOTAL_SIZE_DMA / dma_size);
  //unsigned int njobs = NJDS * 16;
  unsigned int nsubmitted = 0;
  unsigned int ncompleted = 0;
  gc_job_desc *all_jds[NJDS];
  gc_job_desc *jds[2][NJDS];
  unsigned int njds[2];
  unsigned int ci;		// current index
  bool done[NJDS];
  
  static const unsigned int BUFSIZE = (32 << 10) * NJDS;
  unsigned char *getbuf = new unsigned char[BUFSIZE];
  boost::scoped_array<unsigned char> _getbuf(getbuf);
  unsigned char *putbuf = new unsigned char[BUFSIZE];
  boost::scoped_array<unsigned char> _putbuf(putbuf);
  int gbi = 0;

  // touch all pages to force allocation now
  for (unsigned int i = 0; i < BUFSIZE; i += 4096){
    getbuf[i] = 0;
    putbuf[i] = 0;
  }

  gc_jm_options opts;
  opts.program_handle = gc_program_handle_from_address(&benchmark_procs);
  opts.nspes = nspes;
  //opts.enable_logging = true;
  //opts.log2_nlog_entries = 13;
  gc_job_manager_sptr mgr = gc_make_job_manager(&opts);

  if ((gcp_benchmark_udelay = mgr->lookup_proc("benchmark_udelay")) == GCP_UNKNOWN_PROC){
    fprintf(stderr, "lookup_proc: failed to find \"benchmark_udelay\"\n");
    return;
  }

  // allocate and init all job descriptors
  for (int i = 0; i < NJDS; i++){
    if (gbi + dma_size > BUFSIZE)
      gbi = 0;

    all_jds[i] = mgr->alloc_job_desc();
    if (all_jds[i] == 0){
      fprintf(stderr, "alloc_job_desc() returned 0\n");
      return;
    }
    init_jd(all_jds[i], usecs, &getbuf[gbi], &putbuf[gbi], dma_size, getput_mask);
    gbi += dma_size;
  }

  for (int iter = 0; iter < 1; iter++){

    ptime t_start(microsec_clock::universal_time());

    nsubmitted = 0;
    ncompleted = 0;

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
	  if (jds[ci][i]->status != JS_OK){
	    printf("js_status = %d, job_id = %d, ncompleted = %d\n",
		   jds[ci][i]->status, jds[ci][i]->sys.job_id, ncompleted);
	  }
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
    ptime t_stop(microsec_clock::universal_time());

    double delta = (t_stop - t_start).total_microseconds() * 1e-6;
    printf("nspes: %2d  udelay: %4d  elapsed_time: %7.3f  dma_size: %5d  dma_throughput: %7.3e\n",
	   mgr->nspes(), usecs, delta, dma_size,
	   (double) njobs * dma_size / delta * (getput_mask == BENCHMARK_GET_PUT ? 2.0 : 1.0));

  }
}

static void
usage()
{
  fprintf(stderr, "usage: benchmark_dma [-p] [-g] [-n <nspes>] [-u <udelay>] [-s <dma_size>]\n");
  fprintf(stderr, "  you must specify one or both of -p (put) and -g (get)\n");
}


int
main(int argc, char **argv)
{
  unsigned int nspes = 0;
  unsigned int usecs = 0;
  unsigned int dma_size = 32 << 10;
  int getput_mask = 0;
  int ch;

  while ((ch = getopt(argc, argv, "n:u:s:pg")) != EOF){
    switch(ch){
    case 'n':
      nspes = strtol(optarg, 0, 0);
      break;

    case 'u':
      usecs = strtol(optarg, 0, 0);
      break;

    case 's':
      dma_size = strtol(optarg, 0, 0);
      if (dma_size == 0){
	fprintf(stderr, "-s <dma_size> must be > 0\n");
	return 1;
      }
      break;

    case 'p':
      getput_mask |= BENCHMARK_PUT;
      break;

    case 'g':
      getput_mask |= BENCHMARK_GET;
      break;
      
    case '?':
    default:
      usage();
      return 1;
    }
  }

  if (getput_mask == 0){
    usage();
    return 1;
  }

  run_test(nspes, usecs, dma_size, getput_mask);
  return 0;
}
