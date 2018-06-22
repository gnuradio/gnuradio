/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#include <gnuradio/atsc/interleaver_fifo.h>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(t0) {
  interleaver_fifo<int>* fifo = new interleaver_fifo<int>(0);

  for (int i = 10; i < 20; i++)
    BOOST_REQUIRE_EQUAL(i, fifo->stuff(i));
  delete fifo;
}

BOOST_AUTO_TEST_CASE(t1) {
  interleaver_fifo<int>* fifo = new interleaver_fifo<int>(1);

  BOOST_REQUIRE_EQUAL(0, fifo->stuff(2));

  for (int i = 1; i < 10; i++)
    BOOST_REQUIRE_EQUAL(i * 2, fifo->stuff((i + 1) * 2));
  delete fifo;
}

BOOST_AUTO_TEST_CASE(t2) {
  interleaver_fifo<int>* fifo = new interleaver_fifo<int>(4);

  BOOST_REQUIRE_EQUAL(0, fifo->stuff (1));
  BOOST_REQUIRE_EQUAL(0, fifo->stuff (2));
  BOOST_REQUIRE_EQUAL(0, fifo->stuff (3));
  BOOST_REQUIRE_EQUAL(0, fifo->stuff (4));

  for (int i = 5; i < 20; i++)
    BOOST_REQUIRE_EQUAL(i - 4, fifo->stuff (i));
  delete fifo;
}
