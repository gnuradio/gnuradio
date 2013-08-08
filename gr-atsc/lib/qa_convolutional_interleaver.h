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

#ifndef _QA_CONVOLUTIONAL_INTERLEAVER_H_
#define _QA_CONVOLUTIONAL_INTERLEAVER_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCase.h>

#include <gnuradio/atsc/convolutional_interleaver.h>

class qa_convolutional_interleaver : public CppUnit::TestCase {
 private:
  convolutional_interleaver<int>	*intl;
  convolutional_interleaver<int>	*deintl;

  CPPUNIT_TEST_SUITE (qa_convolutional_interleaver);
  CPPUNIT_TEST (t0);
  CPPUNIT_TEST (t1);
  CPPUNIT_TEST (t2);
  CPPUNIT_TEST (t3);
  CPPUNIT_TEST (t4);
  CPPUNIT_TEST_SUITE_END ();

 public:

  void setUp (){
    intl = 0;
    deintl = 0;
  }

  void tearDown (){
    delete intl;
    intl = 0;
    delete deintl;
    deintl = 0;
  }

 private:

  void t0 ();
  void t1 ();
  void t2 ();
  void t3 ();
  void t4 ();

};


#endif /* _QA_CONVOLUTIONAL_INTERLEAVER_H_ */
