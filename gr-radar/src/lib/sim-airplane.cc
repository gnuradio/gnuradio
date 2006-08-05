/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <stdlib.h>
#include <gr_complex.h>
#include <getopt.h>
#include <gr_misc.h>
#include <limits>
#include <gr_fxpt_nco.h>
#include "time_series.h"
#include "simulation.h"

static const double C = 3e8;	// sped of light, m/s


// ------------------------------------------------------------------------

class delay_line {
  std::vector<gr_complex>	d_z;
  const int			d_mask;
  int				d_newest;
public:
  delay_line(unsigned int max_delay)
    : d_z(gr_rounduppow2(max_delay)), d_mask(d_z.size()-1), d_newest(0)
  {
  }

  void
  push_item(gr_complex x)
  {
    d_newest = (d_newest - 1) & d_mask;
    d_z[d_newest] = x;
  }

  gr_complex
  ref_item(int delay) const 
  {
    return d_z[(d_newest + delay) & d_mask];
  }
};

// ------------------------------------------------------------------------

class my_sim : public simulation
{
  FILE			*d_output;
  time_series		&d_ref;
  unsigned long long	 d_pos;		// position in time series
  delay_line		 d_z;
  dyn_object		*d_tx;		// transmitter (not moving)
  dyn_object		*d_rx0;		// receiver (not moving)
  dyn_object		*d_ac0;		// aircraft (linear motion)
  gr_fxpt_nco		 d_nco0;

  double		d_baseline;		// length of baseline in meters
  double		d_last_slant_range;
  double		d_range_bin;		// meters/range_bin
  float			d_tx_lambda;		// wavelength of tx signals in meters
  float			d_sample_rate;
  float			d_gain;			// linear scale factor

public:
  my_sim(FILE *output, time_series &ref, double timestep, float sample_rate,
	 double tx_freq, double gain_db);
  ~my_sim();

  bool update();
  bool run(long long nsteps);

  bool write_output(gr_complex x)
  {
    return fwrite(&x, sizeof(x), 1, d_output) == 1;
  }
};

my_sim::my_sim(FILE *output, time_series &ref, double timestep,
	       float sample_rate, double tx_freq, double gain_db)
  : simulation(timestep),
    d_output(output), d_ref(ref), d_pos(0), d_z(1024),
    d_range_bin(C * timestep), d_tx_lambda(C/tx_freq), 
    d_sample_rate(sample_rate), d_gain(exp10(gain_db/10))
{
  d_tx = new dyn_object(point(0,0), point(0,0), "Tx");
  d_rx0 = new dyn_object(point(45e3,0), point(0,0), "Rx0");

  //float aircraft_speed =  135;         // meters/sec (~ 300 miles/hr)
  float aircraft_speed =  350;         // meters/sec (~ 750 miles/hr)
  float aircraft_angle =  250 * M_PI/180;
  //point aircraft_pos = point(55e3, 20e3);
  point aircraft_pos = point(55e3-5.54e3, 20e3-15.23e3);
  d_ac0 = new dyn_object(aircraft_pos,
			 point(aircraft_speed * cos(aircraft_angle),
			       aircraft_speed * sin(aircraft_angle)),
			 "Ac0");
  add_object(d_tx);
  add_object(d_rx0);
  add_object(d_ac0);

  d_baseline = dyn_object::distance(*d_tx, *d_rx0);
  d_last_slant_range =
    dyn_object::distance(*d_tx, *d_ac0) + dyn_object::distance(*d_ac0, *d_rx0);  
}

my_sim::~my_sim()
{
}

bool
my_sim::update()
{
  // std::cout << *d_ac0 << std::endl;

  // compute slant_range and slant_range'
  double slant_range =
    dyn_object::distance(*d_tx, *d_ac0) + dyn_object::distance(*d_ac0, *d_rx0);	// meters
  double delta_slant_range = slant_range - d_last_slant_range;
  d_last_slant_range = slant_range;
  double deriv_slant_range_wrt_time = delta_slant_range / timestep();		// m/sec

  // fprintf(stdout, "%10.3f\t%10.3f\n", slant_range, deriv_slant_range_wrt_time);

  // grab new item from input and insert it into delay line
  const gr_complex *in = (const gr_complex *) d_ref.seek(d_pos++, 1);
  if (in == 0)
    return false;
  d_z.push_item(*in);

  // FIXME, may want to interpolate between two bins.
  int int_delay = lrint((slant_range - d_baseline) / d_range_bin);

  gr_complex x = d_z.ref_item(int_delay);

  x = x * d_gain;		// scale amplitude (this includes everything: RCS, antenna gain, losses, etc...)

  // compute doppler and apply it
  float f_doppler = -deriv_slant_range_wrt_time / d_tx_lambda;
  fprintf(stdout, "f_dop: %10.3f\n", f_doppler);

  d_nco0.set_freq(f_doppler / d_sample_rate);
  gr_complex phasor(d_nco0.cos(), d_nco0.sin());
  // x = x * phasor;
  d_nco0.step();

  write_output(x);

  return simulation::update();		// run generic update
}

bool
my_sim::run(long long nsteps)
{
  //fprintf(stdout, "<%12.2f, %12.2f>\n", d_ac0->pos().x(), d_ac0->pos().y());
  //std::cout << *d_ac0 << std::endl;
  bool ok = simulation::run(nsteps);
  //std::cout << *d_ac0 << std::endl;
  //fprintf(stdout, "<%12.2f, %12.2f>\n", d_ac0->pos().x(), d_ac0->pos().y());
  return ok;
}

// ------------------------------------------------------------------------

static void
usage(const char *argv0)
{
  const char *progname;
  const char *t = std::strrchr(argv0, '/');
  if (t != 0)
    progname = t + 1;
  else
    progname = argv0;
    
  fprintf(stderr, "usage: %s [options] ref_file\n", progname);
  fprintf(stderr, "    -o OUTPUT_FILENAME [default=sim.dat]\n");
  fprintf(stderr, "    -n NSAMPLES_TO_PRODUCE [default=+inf]\n");
  fprintf(stderr, "    -s NSAMPLES_TO_SKIP [default=0]\n");
  fprintf(stderr, "    -g reflection gain in dB (should be <= 0) [default=0]\n");
  fprintf(stderr, "    -f transmitter freq in Hz [default=100MHz]\n");
  fprintf(stderr, "    -r sample rate in Hz [default=250kHz]\n");
}

int
main(int argc, char **argv)
{
  int	ch;
  const char *output_filename = "sim.dat";
  const char *ref_filename = 0;
  long long int nsamples_to_skip = 0;
  long long int nsamples_to_produce = std::numeric_limits<long long int>::max();
  double sample_rate = 250e3;
  double gain_db = 0;
  double tx_freq = 100e6;

  while ((ch = getopt(argc, argv, "o:s:n:g:f:")) != -1){
    switch (ch){
    case 'o':
      output_filename = optarg;
      break;
      
    case 's':
      nsamples_to_skip = (long long) strtof(optarg, 0);
      if (nsamples_to_skip < 0){
	usage(argv[0]);
	fprintf(stderr, "    nsamples_to_skip must be >= 0\n");
	exit(1);
      }
      break;

    case 'n':
      nsamples_to_produce = (long long) strtof(optarg, 0);
      if (nsamples_to_produce < 0){
	usage(argv[0]);
	fprintf(stderr, "    nsamples_to_produce must be >= 0\n");
	exit(1);
      }
      break;

    case 'g':
      gain_db = strtof(optarg, 0);
      break;

    case 'f':
      tx_freq = strtof(optarg, 0);
      break;

    case 'r':
      sample_rate = strtof(optarg, 0);
      break;

    case '?':
    case 'h':
    default:
      usage(argv[0]);
      exit(1);
    }
  } // while getopt

  if (argc - optind != 1){
    usage(argv[0]);
    exit(1);
  }

  ref_filename = argv[optind++];

  double timestep = 1.0/sample_rate;


  FILE *output = fopen(output_filename, "wb");
  if (output == 0){
    perror(output_filename);
    exit(1);
  }

  unsigned long long ref_starting_offset = 0;
  ref_starting_offset += nsamples_to_skip;

  try {
    time_series ref(sizeof(gr_complex), ref_filename, ref_starting_offset, 0);

    my_sim	simulator(output, ref, timestep, sample_rate, tx_freq, gain_db);
    simulator.run(nsamples_to_produce);
  }
  catch (std::string &s){
    std::cerr << s << std::endl;
    exit(1);
  }

  return 0;
}

