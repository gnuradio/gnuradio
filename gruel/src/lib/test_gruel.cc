/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
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

#include <stdlib.h>
#include <sys/stat.h>

#include "pmt/qa_pmt.h"

static void get_unittest_path (const char *filename, char *fullpath, size_t pathsize);

int 
main(int argc, char **argv)
{
  char path[200];
  get_unittest_path ("gruel.xml", path, 200);
  
  CppUnit::TextTestRunner runner;
  std::ofstream xmlfile(path);
  CppUnit::XmlOutputter *xmlout = new CppUnit::XmlOutputter(&runner.result(), xmlfile);

  runner.addTest(qa_pmt::suite ());
  runner.setOutputter(xmlout);
  
  bool was_successful = runner.run("", false);

  return was_successful ? 0 : 1;
}


// NOTE: These are defined in gr_unittest.h for the rest of the project;
// rewriting here since we don't depend on gnuradio-core in gruel

#ifdef MKDIR_TAKES_ONE_ARG
#define gr_mkdir(pathname, mode) mkdir(pathname)
#else
#define gr_mkdir(pathname, mode) mkdir((pathname), (mode))
#endif

/*
 * Mostly taken from gr_preferences.cc/h
 * The simplest thing that could possibly work:
 *  the key is the filename; the value is the file contents.
 */

static void
ensure_unittest_path (const char *path)
{
  struct stat statbuf;
  if (stat (path, &statbuf) == 0 && S_ISDIR (statbuf.st_mode))
    return;

  // blindly try to make it 	// FIXME make this robust. C++ SUCKS!
  gr_mkdir (path, 0750);
}

static void
get_unittest_path (const char *filename, char *fullpath, size_t pathsize)
{
  char path[200];
  snprintf (path, sizeof(path), "./.unittests");
  snprintf (fullpath, pathsize, "%s/%s", path, filename);

  ensure_unittest_path(path);
}

