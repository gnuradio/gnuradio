/* -*- c++ -*- */
/*
 * Copyright 2003,2004,2008,2009 Free Software Foundation, Inc.
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

#include <iostream>
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
#include <boost/program_options.hpp>

enum {
  GR_SIN_WAVE,
  GR_CONST_WAVE
};

namespace po = boost::program_options;

char *prog_name;

usrp_subdev_spec
str_to_subdev(std::string spec_str)
{
  usrp_subdev_spec spec;
  if(spec_str == "A" || spec_str == "A:0" || spec_str == "0:0") {
    spec.side = 0;
    spec.subdev = 0;
  }
  else if(spec_str == "A:1" || spec_str == "0:1") {
    spec.side = 0;
    spec.subdev = 1;
  }
  else if(spec_str == "B" || spec_str == "B:0" || spec_str == "1:0") {
    spec.side = 1;
    spec.subdev = 0;
  }
  else if(spec_str == "B:1" || spec_str == "1:1") {
    spec.side = 1;
    spec.subdev = 1;
  }
  else {
    throw std::range_error("Incorrect subdevice specifications.\n");
  }

  return spec;
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
die (const char *msg)
{
  fprintf (stderr, "die: %s: %s\n", prog_name, msg);
  exit (1);
}


static bool
test_output (usrp_standard_tx_sptr utx, long long max_bytes, double ampl, int waveform)
{
  const int BUFSIZE = utx->block_size();
  const int N = BUFSIZE/sizeof (short);

  short    	buf[N];
  long long	nbytes = 0;

  // ----------------------------------------------------------------
  // one time initialization of the pattern we're going to transmit

  const int	PERIOD = 65;		// any value is valid
  const int    	PATLEN = 2 * PERIOD;	
  short	       	pattern[PATLEN];

  for (int i = 0; i < PERIOD; i++){
    if (waveform == GR_CONST_WAVE){
      pattern[2*i+0] = host_to_usrp_short((short) ampl);
      pattern[2*i+1] = host_to_usrp_short((short) 0);
    }
    else {
      pattern[2*i+0] = host_to_usrp_short((short) (ampl * cos(2*M_PI * i / PERIOD)));
      pattern[2*i+1] = host_to_usrp_short((short) (ampl * sin(2*M_PI * i / PERIOD)));
    }
  }

  // ----------------------------------------------------------------

  double start_wall_time = get_elapsed_time ();
  double start_cpu_time  = get_cpu_usage ();

  bool 	underrun;
  int 	nunderruns = 0;
  int 	pi = 0;

  for (nbytes = 0; max_bytes == 0 || nbytes < max_bytes; nbytes += BUFSIZE){

    for (int i = 0; i < N; i++){
      buf[i] = pattern[pi];
      pi++;
      if (pi >= PATLEN)
	pi = 0;
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

int
main (int argc, char **argv)
{
  int which = 0;                       // specify which USRP board
  usrp_subdev_spec spec(0,0);          // specify the d'board side
  int interp = 16;                     // set the interpolation rate
  double rf_freq = -1;                 // set the frequency
  float amp = 10000;                   // set the amplitude of the output
  float gain = -1;                     // set the d'board PGA gain
  int waveform;
  int	fusb_block_size = 0;
  int	fusb_nblocks = 0;
  bool	realtime_p = false;
  double nsamples = 32e6;

  set_progname (argv[0]);

  po::options_description cmdconfig("Program options");
  cmdconfig.add_options()
    ("help,h", "produce help message")
    ("which,W", po::value<int>(&which), "select which USRP board")
    ("tx-subdev-spec,T", po::value<std::string>(), "select USRP Tx side A or B")
    ("rf-freq,f", po::value<double>(), "set RF center frequency to FREQ")
    ("interp,i", po::value<int>(&interp), "set fgpa interpolation rate to INTERP")

    ("sine", "generate a complex sinusoid [default]")
    ("const", "generate a constant output")

    //("waveform-freq,w", po::value<double>(&wfreq), "set waveform frequency to FREQ")
    ("amplitude,a", po::value<float>(&amp), "set amplitude")
    ("gain,g", po::value<float>(&gain), "set output gain to GAIN [default=MAX]")
    //("offset,o", po::value<float>(&offset), "set waveform offset to OFFSET")
    ("nsamples,N", po::value<double>(&nsamples), "number of samples to send [default=32M]")
    ;
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(cmdconfig).run(), vm);
  po::notify(vm);
  
  if (vm.count("help")) {
    std::cout << cmdconfig << "\n";
    return 1;
  }
  
  if(vm.count("tx-subdev-spec")) {
    std::string s = vm["tx-subdev-spec"].as<std::string>();
    spec = str_to_subdev(s);
  }

  if(vm.count("sine")) {
    waveform = GR_SIN_WAVE;
  }
  else if(vm.count("const")) {
    waveform = GR_CONST_WAVE;
  }
  else {
    waveform = GR_SIN_WAVE;
  }

  printf("which:    %d\n", which);
  printf("interp:   %d\n", interp);
  printf("rf_freq:  %g\n", rf_freq);
  printf("amp:      %f\n", amp);
  printf("nsamples: %g\n", nsamples);


  if (realtime_p){
    // FIXME
  }

  usrp_standard_tx_sptr utx;

  utx = usrp_standard_tx::make (which,
				interp,
				1, 	// nchan
				-1, 	// mux
				fusb_block_size,
				fusb_nblocks);

  if (utx == 0)
    die ("usrp_standard_tx::make");

  // FIXME

  db_base_sptr subdev = utx->selected_subdev(spec);
  printf("Subdevice name is %s\n", subdev->name().c_str());
  printf("Subdevice freq range: (%g, %g)\n", 
	 subdev->freq_min(), subdev->freq_max());

  unsigned int mux = utx->determine_tx_mux_value(spec);
  printf("mux: %#08x\n",  mux);
  utx->set_mux(mux);

  if(gain == -1)
    gain = subdev->gain_max();
  subdev->set_gain(gain);

  float input_rate = utx->dac_rate() / utx->interp_rate();
  printf("baseband rate: %g\n",  input_rate);

  usrp_tune_result r;

  if (rf_freq < 0)
    rf_freq = (subdev->freq_min() + subdev->freq_max()) * 0.5;
  double target_freq = rf_freq;
  bool ok = utx->tune(subdev->which(), subdev, target_freq, &r);

  if(!ok) {
    throw std::runtime_error("Could not set frequency.");
  }

  subdev->set_enable(true);
  
  printf("target_freq:     %f\n", target_freq);
  printf("ok:              %s\n", ok ? "true" : "false");
  printf("r.baseband_freq: %f\n", r.baseband_freq);
  printf("r.dxc_freq:      %f\n", r.dxc_freq);
  printf("r.residual_freq: %f\n", r.residual_freq);
  printf("r.inverted:      %d\n", r.inverted);


  fflush (stdout);
  fflush (stderr);

  utx->start();		// start data xfers

  test_output (utx, (long long) nsamples, amp, waveform);

  return 0;
}  


#if 0
    case 'B':
      fusb_block_size = strtol (optarg, 0, 0);
      break;

    case 'N':
      fusb_nblocks = strtol (optarg, 0, 0);
      break;

    case 'R':
      realtime_p = true;
      break;

#endif
