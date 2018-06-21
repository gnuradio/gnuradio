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

#include <gnuradio/math.h>
#include <boost/test/unit_test.hpp>
#include <stdio.h>
#include <cmath>

BOOST_AUTO_TEST_CASE(test_binary_slicer1) {
  float x[5] = {-1, -0.5, 0, 0.5, 1.0};
  unsigned int z[5] = {0, 0, 1, 1, 1};
  unsigned int y;

  //printf("\nBinary\n");
  for(unsigned int i = 0; i < 5; i++) {
    y = gr::binary_slicer(x[i]);
    //printf("in: %f   out: %d   desired: %d\n", x[i], y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }

  //printf("\nBranchless Binary\n");
  for (unsigned int i = 0; i < 5; i++) {
    y = gr::branchless_binary_slicer(x[i]);
    //printf("in: %f   out: %d   desired: %d\n", x[i], y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }
}

BOOST_AUTO_TEST_CASE(test_quad_0deg_slicer1) {
  gr_complex x[4] = {gr_complex(1, 0),
		     gr_complex(0, 1),
		     gr_complex(-1, 0),
		     gr_complex(0, -1)};

  unsigned int z[4] = {0, 1, 2, 3};
  unsigned int y;

  //printf("\nQuad0\n");
  for (unsigned int i = 0; i < 4; i++) {
    y = gr::quad_0deg_slicer(x[i]);
    //printf("in: %.4f+j%.4f   out: %d   desired: %d\n", x[i].real(), x[i].imag(), y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }

  //printf("\nBranchless Quad0\n");
  for (unsigned int i = 0; i < 4; i++) {
    y = gr::branchless_quad_0deg_slicer(x[i]);
    //printf("in: %.4f+j%.4f   out: %d   desired: %d\n", x[i].real(), x[i].imag(), y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }
}

BOOST_AUTO_TEST_CASE(test_quad_45deg_slicer1) {
  gr_complex x[4] = {gr_complex(0.707, 0.707),
		     gr_complex(-0.707, 0.707),
		     gr_complex(-0.707, -0.707),
		     gr_complex(0.707, -0.707)};

  unsigned int z[4] = {0, 1, 2, 3};
  unsigned int y;

  //printf("\nQuad45\n");
  for (unsigned int i = 0; i < 4; i++) {
    y = gr::quad_45deg_slicer(x[i]);
    //printf("in: %.4f+j%.4f   out: %d   desired: %d\n", x[i].real(), x[i].imag(), y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }

  //printf("\nBranchless Quad45\n");
  for (unsigned int i = 0; i < 4; i++) {
    y = gr::branchless_quad_45deg_slicer(x[i]);
    //printf("in: %.4f+j%.4f   out: %d   desired: %d\n", x[i].real(), x[i].imag(), y, z[i]);

    BOOST_CHECK_EQUAL(y, z[i]);
  }
}
