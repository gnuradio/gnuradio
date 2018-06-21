/* -*- c++ -*- */
/*
 * Copyright 2002,2013,2018 Free Software Foundation, Inc.
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

#include "circular_file.h"
#include <boost/test/unit_test.hpp>
#include <unistd.h>

static const char *test_file = "qa_gr_circular_file.data";
static const int BUFFER_SIZE = 8192;
static const int NWRITE = 8192 * 9 / 8;

BOOST_AUTO_TEST_CASE(t1) {
#ifdef HAVE_MMAP
  gr::circular_file *cf_writer;
  gr::circular_file *cf_reader;

  // write the data...
  cf_writer = new gr::circular_file(test_file, true,
                                    BUFFER_SIZE * sizeof(short));

  short sd;
  for(int i = 0; i < NWRITE; i++) {
    sd = i;
    cf_writer->write(&sd, sizeof (sd));
  }

  delete cf_writer;

  // now read it back...
  cf_reader = new gr::circular_file(test_file);
  for(int i = 0; i < BUFFER_SIZE; i++) {
    int n = cf_reader->read (&sd, sizeof(sd));
    BOOST_CHECK_EQUAL((int) sizeof (sd), n);
    BOOST_CHECK_EQUAL(NWRITE - BUFFER_SIZE + i, (int)sd);
  }

  int n = cf_reader->read(&sd, sizeof(sd));
  BOOST_CHECK_EQUAL(0, n);

  delete cf_reader;
  unlink(test_file);
#endif // HAVE_MMAP
}

