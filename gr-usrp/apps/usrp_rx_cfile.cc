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

#include <usrp_rx_cfile.h>
#include <gr_io_signature.h>
#include <gr_head.h>
#include <stdexcept>
#include <iostream>
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
usrp_rx_cfile_sptr make_usrp_rx_cfile(int which, usrp_subdev_spec spec, 
				      int decim, double freq, float gain,
				      bool width8, bool nohb,
				      bool output_shorts, int nsamples,
				      const std::string &filename)
{
  return gnuradio::get_initial_sptr(new usrp_rx_cfile(which, spec, 
						      decim, freq, gain,
						      width8, nohb,
						      output_shorts, 
						      nsamples,
						      filename));
}

// Hierarchical block constructor, with no inputs or outputs
usrp_rx_cfile::usrp_rx_cfile(int which, usrp_subdev_spec spec, 
			     int decim, double freq, float gain,
			     bool width8, bool nohb,
			     bool output_shorts, int nsamples,
			     const std::string &filename) : 
  gr_top_block("usrp_rx_cfile"),
  d_which(which),  d_spec(spec), d_decim(decim), d_freq(freq), 
  d_gain(gain), d_width8(width8), d_nohb(nohb), d_nsamples(nsamples), 
  d_filename(filename)
{
  usrp_source_c_sptr usrp;

  if(d_nohb || (d_decim<8)) {
    // Min decimation of this firmware is 4. 
    // contains 4 Rx paths without halfbands and 0 tx paths.
    std::string fpga_filename="std_4rx_0tx.rbf";

    // use default values and add fpga_filename
    usrp = usrp_make_source_c(d_which, d_decim,
			      1, -1, 0, 0, 0,
			      fpga_filename.c_str());
  }
  else {
    // standard fpga firmware "std_2rxhb_2tx.rbf" contains 
    // 2 Rx paths with halfband filters and 2 tx paths 
    //(the default) min decimation 8
    usrp = usrp_make_source_c(d_which, d_decim);
  }

  if(d_width8) {
    int sample_width = 8;
    int sample_shift = 8;
    int format = usrp->make_format(sample_width, sample_shift);
    int r = usrp->set_format(format);
    printf("width8: format=%d  r=%d\n", format, r);
  }


  /* Get subdevice and process it */
  db_base_sptr subdev = usrp->selected_subdev(d_spec);
  printf("\nSubdevice name is %s\n", subdev->side_and_name().c_str());
  printf("Subdevice freq range: (%g, %g)\n", 
	 subdev->freq_min(), subdev->freq_max());

  unsigned int mux = usrp->determine_rx_mux_value(d_spec);
  printf("mux: %#08x\n",  mux);
  usrp->set_mux(mux);

  float gain_min = subdev->gain_min();
  float gain_max = subdev->gain_max();
  if(d_gain == -1) {
    d_gain = (gain_min + gain_max)/2.0;
  }
  printf("gain: %g\n", d_gain);
  subdev->set_gain(d_gain);

    
  /* Set the USRP/dboard frequency */
  usrp_tune_result r;
  bool ok = usrp->tune(0, subdev, freq, &r); //DDC 0
  
  if(!ok) {
    throw std::runtime_error("Could not set frequency.");
  }

  /* The rest */
  d_dst = gr_make_file_sink(sizeof(gr_complex), d_filename.c_str());

  if(d_nsamples == -1) {
    connect(usrp, 0, d_dst, 0);
  }
  else {
    d_head = gr_make_head(sizeof(gr_complex), d_nsamples*2);
    connect(usrp, 0, d_head, 0);
    connect(d_head, 0, d_dst, 0);
  }
}


int main(int argc, char *argv[])
{
  int which = 0;                       // specify which USRP board
  usrp_subdev_spec spec(0,0);          // specify the d'board side
  int decim = 16;                      // set the decimation rate
  double freq = 0;                     // set the frequency
  float gain = -1;                     // set the gain; -1 will set the mid-point gain
  int nsamples = -1;                   // set the number of samples to collect; -1 will continue
  bool width8 = false;                 // use 8-bit samples across USB
  bool nohb = false;                   // don't use halfband filter in USRP
  bool output_shorts = false;          // use shorts
  std::string filename = "received.dat";

  po::options_description cmdconfig("Program options: usrp_text_rx [options] filename");
  cmdconfig.add_options()
    ("help,h", "produce help message")
    ("which,W", po::value<int>(&which), "select which USRP board")
    ("rx-subdev-spec,R", po::value<std::string>(), "select USRP Rx side A or B (default=A)")
    ("decim,d", po::value<int>(&decim), "set fgpa decimation rate to DECIM")
    ("freq,f", po::value<double>(), "set frequency to FREQ")
    ("gain,g", po::value<float>(), "set gain in dB (default is midpoint)")
    ("width-8,8", "Enable 8-bit samples across USB")
    ("no-hb", "don't use halfband filter in usrp")
    //("output-shorts,s", "output interleaved shorts in stead of complex floats")
    ("nsamples,N", po::value<int>(&nsamples), "number of samples to collect")
    ;

  po::options_description fileconfig("Input file options");
  fileconfig.add_options()
    ("filename", po::value<std::string>(), "input file")
    ;

  po::positional_options_description inputfile;
  inputfile.add("filename", -1);

  po::options_description config;
  config.add(cmdconfig).add(fileconfig);
  
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
	    options(config).positional(inputfile).run(), vm);
  po::notify(vm);
  
  if (vm.count("help")) {
    std::cout << cmdconfig << "\n";
    return 1;
  }

  if(vm.count("filename")) {
    filename = vm["filename"].as<std::string>();
  }
  
  if(vm.count("freq")) {
    freq = vm["freq"].as<double>();
  }
  else {
    fprintf(stderr, "You must specify a frequency.\n");
    return -1;
  }

  if(vm.count("rx-subdev-spec")) {
    std::string s = vm["rx-subdev-spec"].as<std::string>();
    spec = str_to_subdev(s);
  }

  if(vm.count("width-8")) {
    width8 = true;
  }
  if(vm.count("nohb")) {
    nohb = true;
  }
  if(vm.count("output-shorts")) {
    output_shorts = true;
  }

  std::cout << "which:   " << which << std::endl;
  std::cout << "decim:   " << decim << std::endl;
  std::cout << "freq:    " << freq << std::endl;
  std::cout << "gain:    " << gain << std::endl;
  std::cout << "width-8  " << (width8 ? "Yes" : "No") << std::endl;
  std::cout << "no-hb    " << (nohb ? "Yes" : "no") << std::endl;
  std::cout << "shorts:  " << (output_shorts ? "Yes" : "No") << std::endl;
  std::cout << "samples: " << nsamples << std::endl;

  usrp_rx_cfile_sptr top_block = make_usrp_rx_cfile(which, spec, decim, freq, 
						    gain, width8, nohb, 
						    output_shorts, nsamples,
						    filename);
  top_block->run();
    
  return 0;
}
