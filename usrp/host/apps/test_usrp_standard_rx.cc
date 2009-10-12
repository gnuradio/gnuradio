/* -*- c++ -*- */
/*
 * Copyright 2003,2006,2008,2009 Free Software Foundation, Inc.
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
#include <getopt.h>
#include <assert.h>
#include <math.h>
#include "time_stuff.h"
#include <usrp/usrp_standard.h>
#include <usrp/usrp_bytesex.h>
#include "fpga_regs_common.h"
#include "fpga_regs_standard.h"

#ifdef HAVE_SCHED_H
#include <sched.h>
#endif

char *prog_name;

static bool test_input  (usrp_standard_rx_sptr urx, int max_bytes, FILE *fp);

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
  fprintf (stderr, "usage: %s [-f] [-v] [-l] [-c] [-D <decim>] [-F freq] [-o output_file]\n", prog_name);
  fprintf (stderr, "  [-f] loop forever\n");
  fprintf (stderr, "  [-M] how many Megabytes to transfer (default 128)\n");
  fprintf (stderr, "  [-v] verbose\n");
  fprintf (stderr, "  [-l] digital loopback in FPGA\n");
  fprintf (stderr, "  [-c] counting in FPGA\n");
  fprintf (stderr, "  [-8] 8-bit samples across USB\n");
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

int
main (int argc, char **argv)
{
  bool 	verbose_p = false;
  bool	loopback_p = false;
  bool  counting_p = false;
  bool	width_8_p = false;
  int   max_bytes = 128 * (1L << 20);
  int	ch;
  char	*output_filename = 0;
  int	which_board = 0;
  int	decim = 8;			// 32 MB/sec
  double	center_freq = 0;
  int	fusb_block_size = 0;
  int	fusb_nblocks = 0;
  bool	realtime_p = false;


  set_progname (argv[0]);

  while ((ch = getopt (argc, argv, "fvlco:D:F:M:8B:N:R")) != EOF){
    switch (ch){
    case 'f':
      max_bytes = 0;
      break;

    case 'v':
      verbose_p = true;
      break;

    case 'l':
      loopback_p = true;
      break;

    case 'c':
      counting_p = true;
      break;
      
    case '8':
      width_8_p = true;
      break;
      
    case 'o':
      output_filename = optarg;
      break;
      
    case 'D':
      decim = strtol (optarg, 0, 0);
      break;

    case 'F':
      center_freq = strtod (optarg, 0);
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

  FILE *fp = 0;

  if (output_filename){
    fp = fopen (output_filename, "wb");
    if (fp == 0)
      perror (output_filename);
  }
      
  int mode = 0;
  if (loopback_p)
    mode |= usrp_standard_rx::FPGA_MODE_LOOPBACK;
  if (counting_p)
    mode |= usrp_standard_rx::FPGA_MODE_COUNTING;


  usrp_standard_rx_sptr urx = 
    usrp_standard_rx::make (which_board, decim, 1, -1, mode,
			    fusb_block_size, fusb_nblocks);

  if (urx == 0)
    die ("usrp_standard_rx::make");

  if (!urx->set_rx_freq (0, center_freq))
    die ("urx->set_rx_freq");
    
  if (width_8_p){
    int width = 8;
    int shift = 8;
    bool want_q = true;
    if (!urx->set_format(usrp_standard_rx::make_format(width, shift, want_q)))
      die("urx->set_format");
  }

  urx->start();		// start data xfers

  test_input (urx, max_bytes, fp);

  if (fp)
    fclose (fp);

  return 0;
}


static bool
test_input  (usrp_standard_rx_sptr urx, int max_bytes, FILE *fp)
{
  int		   fd = -1;
  static const int BUFSIZE = urx->block_size();
  static const int N = BUFSIZE/sizeof (short);
  short 	   buf[N];
  int		   nbytes = 0;

  double	   start_wall_time = get_elapsed_time ();
  double	   start_cpu_time  = get_cpu_usage ();

  if (fp)
    fd = fileno (fp);
  
  bool overrun;
  int noverruns = 0;

  for (nbytes = 0; max_bytes == 0 || nbytes < max_bytes; nbytes += BUFSIZE){

    unsigned int	ret = urx->read (buf, sizeof (buf), &overrun);
    if (ret != sizeof (buf)){
      fprintf (stderr, "test_input: error, ret = %d\n", ret);
    }

    if (overrun){
      printf ("rx_overrun\n");
      noverruns++;
    }
    
    if (fd != -1){

      for (unsigned int i = 0; i < sizeof (buf) / sizeof (short); i++)
	buf[i] = usrp_to_host_short (buf[i]);

      if (write (fd, buf, sizeof (buf)) == -1){
	perror ("write");
	fd = -1;
      }
    }
  }

  double stop_wall_time = get_elapsed_time ();
  double stop_cpu_time  = get_cpu_usage ();

  double delta_wall = stop_wall_time - start_wall_time;
  double delta_cpu  = stop_cpu_time  - start_cpu_time;

  printf ("xfered %.3g bytes in %.3g seconds.  %.4g bytes/sec.  cpu time = %.4g\n",
	  (double) max_bytes, delta_wall, max_bytes / delta_wall, delta_cpu);
  printf ("noverruns = %d\n", noverruns);

  return true;
}
