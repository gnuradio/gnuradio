/* -*- c++ -*- */
/*
 * Copyright 2007,2010 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include <gr_unittests.h>
#include "../lib/runtime/qa_gcell_runtime.h"
#include "../lib/wrapper/qa_gcell_wrapper.h"

int 
main(int argc, char **argv)
{
  char path[200];
  get_unittest_path ("gcell_all.xml", path, 200);

  CppUnit::TextTestRunner runner;
  std::ofstream xmlfile(path);
  CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

  runner.addTest(qa_gcell_runtime::suite());
  runner.addTest(qa_gcell_wrapper::suite());
  runner.setOutputter(xmlout);
  
  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
}
