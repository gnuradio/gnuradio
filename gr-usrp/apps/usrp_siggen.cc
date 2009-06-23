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
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include <usrp_siggen.h>
#include <gr_io_signature.h>
#include <gr_head.h>
#include <gr_noise_type.h>
#include <stdexcept>
#include <iostream>
#include <cstdio>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

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

// Shared pointer constructor
usrp_siggen_sptr make_usrp_siggen(int which, usrp_subdev_spec spec, 
				  double rf_freq, int interp, double wfreq,
				  int waveform, float amp, float gain, 
				  float offset, long long nsamples)
{
  return gnuradio::get_initial_sptr(new usrp_siggen(which, spec, 
						    rf_freq, interp, wfreq,
						    waveform, amp, gain, 
						    offset, nsamples));
}

// Hierarchical block constructor, with no inputs or outputs
usrp_siggen::usrp_siggen(int which, usrp_subdev_spec spec, 
			 double rf_freq, int interp, double wfreq,
			 int waveform, float amp, float gain, 
			 float offset, long long nsamples)
  : gr_top_block("usrp_siggen")
{
  usrp_sink_c_sptr usrp = usrp_make_sink_c(which, interp);

  db_base_sptr subdev = usrp->selected_subdev(spec);
  printf("Subdevice name is %s\n", subdev->name().c_str());
  printf("Subdevice freq range: (%g, %g)\n", 
	 subdev->freq_min(), subdev->freq_max());

  unsigned int mux = usrp->determine_tx_mux_value(spec);
  printf("mux: %#08x\n",  mux);
  usrp->set_mux(mux);

  if(gain == -1) {
    gain = subdev->gain_max();
  }
  subdev->set_gain(gain);

  float input_rate = usrp->dac_freq() / usrp->interp_rate();
  printf("baseband rate: %g\n",  input_rate);

  usrp_tune_result r;
  double target_freq = rf_freq;
  bool ok = usrp->tune(subdev->which(), subdev, target_freq, &r);

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

  /* Set up the signal source */
  siggen = gr_make_sig_source_c(input_rate, GR_SIN_WAVE, wfreq, amp);
  noisegen = gr_make_noise_source_c (GR_UNIFORM, amp);
  if(waveform == GR_SIN_WAVE || waveform == GR_CONST_WAVE) {
    source = siggen;
  }
  else if(waveform == GR_UNIFORM || waveform == GR_GAUSSIAN) {
    source = noisegen;
  }
  else {
    throw std::range_error("Unknown waveform type.\n");
  }

  siggen->set_waveform((gr_waveform_t)waveform);

  if (nsamples > 0){
    gr_block_sptr head = gr_make_head(sizeof(gr_complex), nsamples);
    connect(source, 0, head, 0);
    connect(head, 0, usrp, 0);
  }
  else {
    connect(source, 0, usrp, 0);
  }
}

int main(int argc, char *argv[])
{
  int which = 0;                       // specify which USRP board
  usrp_subdev_spec spec(0,0);          // specify the d'board side
  int interp = 128;                    // set the interpolation rate
  double rf_freq = 0;                  // set the frequency
  double wfreq = 100e3;                // set the waveform frequency
  float amp = 5;                       // set the amplitude of the output
  float gain = -1;                     // set the d'board PGA gain
  float offset = 0;                    // set waveform offset
  int waveform;
  double nsamples = 0;		       // set the number of samples to transmit (0 -> inf)

  po::options_description cmdconfig("Program options");
  cmdconfig.add_options()
    ("help,h", "produce help message")
    ("which,W", po::value<int>(&which), "select which USRP board")
    ("tx-subdev-spec,T", po::value<std::string>(), "select USRP Tx side A or B")
    ("rf-freq,f", po::value<double>(), "set RF center frequency to FREQ")
    ("interp,i", po::value<int>(&interp), "set fgpa interpolation rate to INTERP")

    ("sine", "generate a complex sinusoid [default]")
    ("const", "generate a constant output")
    ("gaussian", "generate Gaussian random output")
    ("uniform", "generate Uniform random output")

    ("waveform-freq,w", po::value<double>(&wfreq), "set waveform frequency to FREQ")
    ("amplitude,a", po::value<float>(&amp), "set amplitude")
    ("gain,g", po::value<float>(&gain), "set output gain to GAIN")
    ("offset,o", po::value<float>(&offset), "set waveform offset to OFFSET")
    ("nsamples,N", po::value<double>(&nsamples), "number of samples to send [default=+inf]")
    ;
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(cmdconfig).run(), vm);
  po::notify(vm);
  
  if (vm.count("help")) {
    std::cout << cmdconfig << "\n";
    return 1;
  }
  
  if(vm.count("rf-freq")) {
    rf_freq = vm["rf-freq"].as<double>();
  }
  else {
    fprintf(stderr, "You must specify a frequency.\n");
    return -1;
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
  else if(vm.count("gaussian")) {
    waveform = GR_GAUSSIAN;
  }
  else if(vm.count("uniform")) {
    waveform = GR_UNIFORM;
  }
  else {
    waveform = GR_SIN_WAVE;
  }

  printf("which:    %d\n", which);
  printf("interp:   %d\n", interp);
  printf("rf_freq:  %g\n", rf_freq);
  printf("amp:      %f\n", amp);
  printf("nsamples: %g\n", nsamples);

  usrp_siggen_sptr top_block = make_usrp_siggen(which, spec, rf_freq, 
						interp, wfreq, waveform,
						amp, gain, offset, (long long) nsamples);

  top_block->run();
  
  return 0;
}
