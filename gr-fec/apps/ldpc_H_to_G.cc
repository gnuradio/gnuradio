/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 3, or (at your
 * option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <boost/shared_ptr.hpp>
#include <gnuradio/fec/fec_mtrx.h>
#include <boost/program_options.hpp>

using namespace gr::fec::code;
namespace po = boost::program_options;

int main(int argc, char **argv)
{
  //setup the program options
  po::options_description desc("This program converts an H matrix from an alist file "
                               "into a G matrix alist file.\n\n"
                               "ldpc_H_to_G [opts] inputfile outputfile\n\n"
                               "Command line options");
  desc.add_options()
    ("help,h", "Produce the help message")
    ("verbose,v", "Turn on verbose mode")
    ;

  po::options_description hidden("Hidden options");
  hidden.add_options()
    ("files", po::value< std::vector<std::string> >(), "Files")
    ;

  po::positional_options_description p;
  p.add("files", -1);

  po::options_description allopts;
  allopts.add(desc).add(hidden);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).options(allopts).
            positional(p).run(), vm);
  po::notify(vm);

  //print the help message
  if(vm.count("help")){
    std::cout << std::endl << desc << std::endl;
    exit(0);
  }

  std::string infilename, outfilename;
  if(vm.count("files")) {
    std::vector<std::string> filelist = vm["files"].as< std::vector<std::string> >();
    if(filelist.size() >= 2) {
      infilename = filelist[0];
      outfilename = filelist[1];
    }
    else {
      std::cerr << "Please specify input and output files." << std::endl;
      std::cerr << std::endl << desc << std::endl;
      exit(1);
    }
  }
  else {
    std::cerr << "Please specify input and output files." << std::endl;
    std::cout << std::endl << desc << std::endl;
    exit(1);
  }

  matrix_sptr H = read_matrix_from_file(infilename);
  matrix_sptr G = generate_G(H);

  write_matrix_to_file(outfilename, G);

  if(vm.count("verbose")) {
    std::cout << "H matrix: nrows: " << H->size1 << std::endl;
    std::cout << "H matrix: ncols: " << H->size2 << std::endl;
    print_matrix(H);

    std::cout << std::endl;
    std::cout << "G matrix: nrows: " << G->size1 << std::endl;
    std::cout << "G matrix: ncols: " << G->size2 << std::endl;
    print_matrix(G);
  }
}
