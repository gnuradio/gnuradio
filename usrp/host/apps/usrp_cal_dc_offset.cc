/* -*- c++ -*- */
/*
 * Copyright 2005,2008,2009 Free Software Foundation, Inc.
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
#include <boost/scoped_ptr.hpp>
#include <usrp/usrp_local_sighandler.h>
#include <usrp/usrp_standard.h>
#include <usrp/usrp_bytesex.h>

char *prog_name;




static void 
run_cal(usrp_standard_rx_sptr u, int which_side, int decim, bool verbose_p)
{
  static const int BUFSIZE = u->block_size();
  static const int N = BUFSIZE/sizeof (short);
  short 	   buf[N];
  bool 	cal_done = false;
  bool overrun;
  int  noverruns = 0;

  static const double K = 1e-4;
  long	integrator[2];
  int	offset[2];

  integrator[0] = 0;
  integrator[1] = 0;
  offset[0] = 0;
  offset[1] = 0;

  u->start();		// start data xfers

  while(!cal_done){
    int	ret = u->read (buf, sizeof (buf), &overrun);
    if (ret != (int) sizeof (buf)){
      fprintf (stderr, "usrp_cal_dc_offset: error, ret = %d\n", ret);
      continue;
    }
    if (overrun){
      fprintf (stderr, "O");
      noverruns++;
    }
    else {
      // fputc('.', stderr);
    }

    static const int MAX = (1L << 30);		// 1G

    for (int i = 0; i < N/2; i++){
      for (int n = 0; n < 2; n++){
	integrator[n] = integrator[n] + buf[2*i + n];
	if (integrator[n] > MAX)
	  integrator[n] = MAX;
	else if (integrator[n] < -MAX)
	  integrator[n] = -MAX;
      }
    }

#if 1
    for (int n = 0; n < 2; n++){
      offset[n] = (int) rint(integrator[n] * K);
      if (offset[n] > 32767)
	offset[n] = 32767;
      else if (offset[n] < -32767)
	offset[n] = -32767;
      u->set_adc_offset(which_side * 2 + n, offset[n]);
    }
#else
    offset[0] = (int) rint(integrator[0] * K);
    if (offset[0] > 32767)
      offset[0] = 32767;
    else if (offset[0] < -32767)
      offset[0] = -32767;
    u->set_adc_offset(which_side * 2 + 0, offset[0]);
    u->set_adc_offset(which_side * 2 + 1, offset[0]);
#endif
    

    printf ("%9ld : %6d\t\t%9ld : %6d\n",
	    integrator[0], offset[0], integrator[1], offset[1]);
  }

  u->stop();
}


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
  fprintf(stderr, "usage: %s [-v] [-w which_side] [-D decim] [-c ddc_freq] [-g gain]\n", prog_name);
  fprintf(stderr, "  [-S fusb_block_size] [-N fusb_nblocks]\n");
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
  int	    ch;
  int	    decim = 128;		// 500 kS/sec
  bool	    verbose_p = false;
  int	    which_board = 0;
  int	    which_side = 0;
  double    ddc_freq = 0;
  int	    fusb_block_size = 1024;
  int	    fusb_nblocks = 4;
  double    pga_gain = 0.0;

  set_progname(argv[0]);

  while ((ch = getopt (argc, argv, "vw:D:c:S:N:g:")) != EOF){
    switch (ch){

    case 'w':
      which_side = strtol (optarg, 0, 0);
      if (which_side < 0 || which_side > 1)
	usage();
      break;

    case 'D':
      decim = strtol (optarg, 0, 0);
      if (decim < 1)
	usage();
      break;

    case 'c':
      ddc_freq = strtod (optarg, 0);
      break;

    case 'v':
      verbose_p = true;
      break;

    case 'S':
      fusb_block_size = strtol(optarg, 0, 0);
      break;

    case 'N':
      fusb_nblocks = strtol(optarg, 0, 0);
      break;

    case 'g':
      pga_gain = strtod (optarg, 0);
      break;

    default:
      usage ();
    }
  }

  int nchannels = 1;
  int mode = usrp_standard_rx::FPGA_MODE_NORMAL;
  int mux;

  if (which_side == 0)
    mux = 0x00000010;
  else
    mux = 0x00000032;

#ifdef SIGINT
  usrp_local_sighandler sigint (SIGINT, usrp_local_sighandler::throw_signal);
#endif
#ifdef SIGQUIT
  usrp_local_sighandler sigquit (SIGQUIT, usrp_local_sighandler::throw_signal);
#endif

  usrp_standard_rx_sptr urx =
    usrp_standard_rx::make(which_board, decim,
			   nchannels, mux, mode,
			   fusb_block_size, fusb_nblocks);
  if (!urx)
    die("usrp_standard_rx::make");

  try {

    if (!urx->set_rx_freq(0, ddc_freq))
      die("urx->set_rx_freq");

    urx->set_pga(2 * which_side + 0, pga_gain);
    urx->set_pga(2 * which_side + 1, pga_gain);
    
    run_cal(urx, which_side, decim, verbose_p);
  }
  catch (usrp_signal &sig){
    fprintf (stderr, "usrp_cal_dc_offset: caught %s\n", sig.name().c_str());
  }
  catch(...){
    fprintf (stderr, "usrp_cal_dc_offset: caught something\n");
  }
}

