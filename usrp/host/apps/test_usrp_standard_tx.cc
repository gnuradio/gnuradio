/* -*- c++ -*- */
/*
 * Copyright 2003,2004 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <usb.h>			/* needed for usb functions */
#include <getopt.h>
#include <assert.h>
#include <math.h>
#include "time_stuff.h"
#include "usrp_standard.h"
#include "usrp_bytesex.h"

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

char *prog_name;

static bool test_output (usrp_standard_tx *utx, int max_bytes, double ampl,
			 bool dc_p, bool counting_p);

static void
set_progname (char *path)
{
  char *p = strrchr (path, '/');
  if (p != 0)
    prog_name = p+1;
  else
    prog_name = path;
}

static void
usage ()
{
  fprintf (stderr, 
	   "usage: %s [-f] [-v] [-d] [-c] [-a <ampl>][-I <interp>] [-F freq] [-D]\n", prog_name);
  fprintf (stderr, "  [-f] loop forever\n");
  fprintf (stderr, "  [-M] how many Megabytes to transfer (default 128)\n");
  fprintf (stderr, "  [-v] verbose\n");
  fprintf (stderr, "  [-d] dump registers\n");
  // fprintf (stderr, "  [-l] digital loopback in FPGA\n");
  fprintf (stderr, "  [-c] Tx counting sequence\n");
  fprintf (stderr, "  [-D] DC output\n");

  fprintf (stderr, "  [-B <fusb_block_size>] set fast usb block_size\n");
  fprintf (stderr, "  [-N <fusb_nblocks>] set fast usb nblocks\n");
  fprintf (stderr, "  [-R] set real time scheduling: SCHED_FIFO; pri = midpoint\n");

  exit (1);
}

static void
die (const char *msg)
{
  fprintf (stderr, "die: %s: %s\n", prog_name, msg);
  exit (1);
}

static void
dump_codec_regs (usrp_basic *u, int which_codec, FILE *fp)
{
  for (int i = 0; i < 64; i++){
    unsigned char v;
    u->_read_9862 (which_codec, i, &v);
    fprintf (fp, "%2d:  0x%02x\n", i, v); 
  }
  fflush (fp);
}

static void
do_dump_codec_regs (usrp_basic *u)
{
  char name[100];
  strcpy (name, "regsXXXXXX");
  int fd = mkstemp (name);
  if (fd == -1){
    perror (name);
  }
  else {
    FILE *fp = fdopen (fd, "w");
    dump_codec_regs (u, 0, fp);
    fclose (fp);
  }
}

int
main (int argc, char **argv)
{
  bool 	verbose_p = false;
  bool  dump_regs_p = false;
  bool	dc_p = false;
  // bool  loopback_p = false;
  bool  counting_p = false;
  int   max_bytes = 128 * (1L << 20);
  int	ch;
  int	which_board = 0;
  int	interp = 16;			// 32.0 MB/sec 
  double	center_freq = 0;
  double	ampl = 10000;
  int	fusb_block_size = 0;
  int	fusb_nblocks = 0;
  bool	realtime_p = false;


  set_progname (argv[0]);

  while ((ch = getopt (argc, argv, "vfdcI:F:a:DM:B:N:R")) != EOF){
    switch (ch){
    case 'f':
      max_bytes = 0;
      break;

    case 'v':
      verbose_p = true;
      break;

    case 'd':
      dump_regs_p = true;
      break;
      
    case 'D':
      dc_p = true;
      break;
      
#if 0
    case 'l':
      loopback_p = true;
      break;
#endif
      
    case 'c':
      counting_p = true;
      break;
      
    case 'I':
      interp = strtol (optarg, 0, 0);
      break;
		      
    case 'F':
      center_freq = strtod (optarg, 0);
      break;
      
    case 'a':
      ampl = strtod (optarg, 0);
      break;

    case 'M':
      max_bytes = strtol (optarg, 0, 0) * (1L << 20);
      if (max_bytes < 0) max_bytes = 0;
      break;

    case 'B':
      fusb_block_size = strtol (optarg, 0, 0);
      break;

    case 'N':
      fusb_nblocks = strtol (optarg, 0, 0);
      break;

    case 'R':
      realtime_p = true;
      break;

    default:
      usage ();
    }
  }

#ifdef HAVE_SCHED_SETSCHEDULER
  if (realtime_p){
    int policy = SCHED_FIFO;
    int pri = (sched_get_priority_max (policy) - sched_get_priority_min (policy)) / 2;
    int pid = 0;  // this process

    struct sched_param param;
    memset(&param, 0, sizeof(param));
    param.sched_priority = pri;
    int result = sched_setscheduler(pid, policy, &param);
    if (result != 0){
      perror ("sched_setscheduler: failed to set real time priority");
    }
    else
      printf("SCHED_FIFO enabled with priority = %d\n", pri);
  }
#endif

  usrp_standard_tx *utx;

  utx = usrp_standard_tx::make (which_board,
				interp,
				1, 	// nchan
				-1, 	// mux
				fusb_block_size,
				fusb_nblocks);

  if (utx == 0)
    die ("usrp_standard_tx::make");
    
  if (!utx->set_tx_freq (0, center_freq))
    die ("utx->set_tx_freq");
    
  if (dump_regs_p)
    do_dump_codec_regs (utx);
  
  
  fflush (stdout);
  fflush (stderr);

  utx->start();		// start data xfers

  test_output (utx, max_bytes, ampl, dc_p, counting_p);

  delete utx;

  return 0;
}


static bool
test_output (usrp_standard_tx *utx, int max_bytes, double ampl,
	     bool dc_p, bool counting_p)
{
  static const int BUFSIZE = utx->block_size();
  static const int N = BUFSIZE/sizeof (short);

  short 	   buf[N];
  int		   nbytes = 0;
  int		   counter = 0;

  static const int    PERIOD = 65;		// any value is valid
  static const int    PATLEN = 2 * PERIOD;	
  short		      pattern[PATLEN];

  for (int i = 0; i < PERIOD; i++){
    if (dc_p){
      pattern[2*i+0] = host_to_usrp_short ((short) ampl);
      pattern[2*i+1] = host_to_usrp_short ((short) 0);
    }
    else {
      pattern[2*i+0] = host_to_usrp_short ((short) (ampl * cos (2*M_PI * i / PERIOD)));
      pattern[2*i+1] = host_to_usrp_short ((short) (ampl * sin (2*M_PI * i / PERIOD)));
    }
  }

  double	   start_wall_time = get_elapsed_time ();
  double	   start_cpu_time  = get_cpu_usage ();

  bool 	underrun;
  int 	nunderruns = 0;
  int 	pi = 0;

  for (nbytes = 0; max_bytes == 0 || nbytes < max_bytes; nbytes += BUFSIZE){

    if (counting_p){
      for (int i = 0; i < N; i++)
	buf[i] = host_to_usrp_short (counter++ & 0xffff);
    }
    else {
      for (int i = 0; i < N; i++){
	buf[i] = pattern[pi];
	pi++;
	if (pi >= PATLEN)
	  pi = 0;
      }
    }

    int	ret = utx->write (buf, sizeof (buf), &underrun);
    if ((unsigned) ret != sizeof (buf)){
      fprintf (stderr, "test_output: error, ret = %d\n", ret);
    }

    if (underrun){
      nunderruns++;
      printf ("tx_underrun\n");
      //printf ("tx_underrun %9d %6d\n", nbytes, nbytes/BUFSIZE);
    }
  }

  utx->wait_for_completion ();

  double stop_wall_time = get_elapsed_time ();
  double stop_cpu_time  = get_cpu_usage ();

  double delta_wall = stop_wall_time - start_wall_time;
  double delta_cpu  = stop_cpu_time  - start_cpu_time;

  printf ("xfered %.3g bytes in %.3g seconds.  %.4g bytes/sec.  cpu time = %.3g\n",
	  (double) max_bytes, delta_wall, max_bytes / delta_wall, delta_cpu);

  printf ("%d underruns\n", nunderruns);

  return true;
}
