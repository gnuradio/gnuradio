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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
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

class aux_state {
public:
  dyn_object	*d_obj;
  double	 d_last_slant_range;
  gr_fxpt_nco	 d_nco;

  aux_state(dyn_object *obj) : d_obj(obj) {}
};

// ------------------------------------------------------------------------

class my_sim : public simulation
{
  FILE			 *d_output;
  time_series		 &d_ref;
  unsigned long long	  d_pos;		// position in time series
  delay_line		  d_z;
  dyn_object		 *d_tx;		// transmitter (not moving)
  dyn_object		 *d_rx0;		// receiver (not moving)
  std::vector<aux_state*> d_target;

  double		  d_baseline;		// length of baseline in meters
  double		  d_range_bin;		// meters/range_bin
  float			  d_tx_lambda;		// wavelength of tx signals in meters
  float			  d_sample_rate;
  float			  d_gain;		// linear scale factor

  void adjust_for_start_time(double start_time);
  
  bool write_output(gr_complex x)
  {
    return fwrite(&x, sizeof(x), 1, d_output) == 1;
  }

public:
  my_sim(FILE *output, time_series &ref, double timestep, float sample_rate,
	 double start_time, double tx_freq, double gain_db);
  ~my_sim();

  bool update();
  bool run(long long nsteps);
};


my_sim::my_sim(FILE *output, time_series &ref, double timestep,
	       float sample_rate, double start_time,
	       double tx_freq, double gain_db)
  : simulation(timestep),
    d_output(output), d_ref(ref), d_pos(0), d_z(1024),
    d_range_bin(C * timestep), d_tx_lambda(C/tx_freq), 
    d_sample_rate(sample_rate), d_gain(exp10(gain_db/10))
{
  d_tx = new dyn_object(point(0,0), point(0,0), "Tx");
  d_rx0 = new dyn_object(point(45e3,0), point(0,0), "Rx0");

  add_object(d_tx);
  add_object(d_rx0);
  d_baseline = dyn_object::distance(*d_tx, *d_rx0);

  {
    // add targets
    float aircraft_speed;
    float aircraft_angle;
    point aircraft_pos;
    dyn_object	*ac;

    // target 1
    aircraft_speed = 135; 			// m/s
    aircraft_angle = 240 * M_PI/180;
    aircraft_pos = point(55e3, 20e3);

    ac = new dyn_object(aircraft_pos,
			point(aircraft_speed * cos(aircraft_angle),
			      aircraft_speed * sin(aircraft_angle)),
			"Ac0");
    add_object(ac);
    d_target.push_back(new aux_state(ac));

    // target 2 
    aircraft_speed = 350; 			// m/s
    aircraft_angle = 0 * M_PI/180;
    aircraft_pos = point(-20e3, 60e3);

    ac = new dyn_object(aircraft_pos,
			point(aircraft_speed * cos(aircraft_angle),
			      aircraft_speed * sin(aircraft_angle)),
			"Ac1");
    add_object(ac);
    d_target.push_back(new aux_state(ac));
  }

  adjust_for_start_time(start_time);

  for (unsigned i = 0; i < d_target.size(); i++)
    d_target[i]->d_last_slant_range =
      (dyn_object::distance(*d_tx, *d_target[i]->d_obj)
       + dyn_object::distance(*d_target[i]->d_obj, *d_rx0));

}

my_sim::~my_sim()
{
}

void
my_sim::adjust_for_start_time(double start_time)
{
  for (unsigned i = 0; i < d_obj.size(); i++){
    // Adjust initial starting positions depending on simulation
    // start time.  FIXME Assumes velocity is constant
    point p = d_obj[i]->pos();
    point v = d_obj[i]->vel();
    p.set_x(p.x() + v.x() * start_time);
    p.set_y(p.y() + v.y() * start_time);
    d_obj[i]->set_pos(p);
  }
}

bool
my_sim::update()
{
  // std::cout << *d_ac0 << std::endl;

  // grab new item from input and insert it into delay line
  const gr_complex *in = (const gr_complex *) d_ref.seek(d_pos++, 1);
  if (in == 0)
    return false;
  d_z.push_item(*in);

  gr_complex s = 0;	// output sample
  // FIXME ought to add in attenuated direct path input


  // for each target, compute slant_range and slant_range'

  for (unsigned i = 0; i < d_target.size(); i++){
    aux_state *t = d_target[i];
    
    double slant_range = 
      (dyn_object::distance(*d_tx, *t->d_obj)
       + dyn_object::distance(*t->d_obj, *d_rx0));			  // meters

    double delta_slant_range = slant_range - t->d_last_slant_range;
    t->d_last_slant_range = slant_range;
    double deriv_slant_range_wrt_time = delta_slant_range / timestep();	  // m/sec

    //fprintf(stdout, "%10.3f\t%10.3f\n", slant_range, deriv_slant_range_wrt_time);

    // FIXME, may want to interpolate between two bins.
    int int_delay = lrint((slant_range - d_baseline) / d_range_bin);

    gr_complex x = d_z.ref_item(int_delay);

    // scale amplitude (this includes everything: RCS, antenna gain, losses, etc...)
    x = x * d_gain;

    if (1){
      // compute doppler and apply it
      float f_doppler = -deriv_slant_range_wrt_time / d_tx_lambda;

      t->d_nco.set_freq(f_doppler / d_sample_rate);
      gr_complex phasor(t->d_nco.cos(), t->d_nco.sin());
      x = x * phasor;
      t->d_nco.step();
    }

    s += x;		// add in this target's contribution
  }

  write_output(s);

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
  fprintf(stderr, "    -S simulation start time in seconds [default=0]\n");
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
  double start_time = 0;

  while ((ch = getopt(argc, argv, "o:s:n:g:f:S:")) != -1){
    switch (ch){
    case 'o':
      output_filename = optarg;
      break;
      
    case 's':
      nsamples_to_skip = (long long) strtod(optarg, 0);
      if (nsamples_to_skip < 0){
	usage(argv[0]);
	fprintf(stderr, "    nsamples_to_skip must be >= 0\n");
	exit(1);
      }
      break;

    case 'n':
      nsamples_to_produce = (long long) strtod(optarg, 0);
      if (nsamples_to_produce < 0){
	usage(argv[0]);
	fprintf(stderr, "    nsamples_to_produce must be >= 0\n");
	exit(1);
      }
      break;

    case 'g':
      gain_db = strtod(optarg, 0);
      break;

    case 'f':
      tx_freq = strtod(optarg, 0);
      break;

    case 'r':
      sample_rate = strtod(optarg, 0);
      break;

    case 'S':
      start_time = strtod(optarg, 0);
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

    my_sim	simulator(output, ref, timestep, sample_rate, start_time,
			  tx_freq, gain_db);
    simulator.run(nsamples_to_produce);
  }
  catch (std::string &s){
    std::cerr << s << std::endl;
    exit(1);
  }

  return 0;
}

