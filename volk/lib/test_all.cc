/* -*- c++ -*- */
/*
 * Copyright 2002,2008 Free Software Foundation, Inc.
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

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TextTestRunner.h>

#include <qa_volk.h>

#include <cppunit/XmlOutputter.h>
#include <iostream>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>

int 
main (int argc, char **argv)
{
  
  int opt = 0;
  std::string xmlOutputFile("");

  while( (opt = getopt(argc, argv, "o:")) != -1){
    switch(opt){
    case 'o':
      if(optarg){
	xmlOutputFile.assign(optarg);
      }
      else{
	std::cerr << "No xml file output specified for -o" << std::endl;
	exit(EXIT_FAILURE);
      }
      break;

    default: /* '?' */
      fprintf(stderr, "Usage: %s [-o] \"xml output file\"\n",
	      argv[0]);
      exit(EXIT_FAILURE);
    }

  }

  CppUnit::TextUi::TestRunner runner;

  runner.addTest (qa_volk::suite ());

  bool was_successful = false;
  if(!xmlOutputFile.empty()){
    std::ofstream xmlOutput(xmlOutputFile.c_str());
    if(xmlOutput.is_open()){
      runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), xmlOutput));

      was_successful = runner.run("", false, true, false);
    }
    xmlOutput.close();
  }
  else{
    was_successful = runner.run ("", false);
  }

  return was_successful ? 0 : 1;
}
