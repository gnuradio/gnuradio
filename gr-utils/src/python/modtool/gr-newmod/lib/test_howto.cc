/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#include <cppunit/TextTestRunner.h>
#include <cppunit/XmlOutputter.h>

#include <gr_unittests.h>
#include "qa_howto.h"
#include <iostream>

int
main (int argc, char **argv)
{
  CppUnit::TextTestRunner runner;
  std::ofstream xmlfile(get_unittest_path("howto.xml").c_str());
  CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

  runner.addTest(qa_howto::suite());
  runner.setOutputter(xmlout);

  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
}
